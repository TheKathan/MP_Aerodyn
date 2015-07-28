'''
Created on 16 Jul 2015

@author: matt
'''

import pi3d
from pi3d.util.OffScreenTexture import OffScreenTexture
from pi3d.shape.FlipSprite import FlipSprite
from MapTile import MapTile
from _dbus_bindings import String
from math import sin, cos, log , pi, ceil
from Line2d import Line2d
import colorsys
from pi3dTiledMap import CoordSys
from pi3dTiledMap.CoordSys import Cartesian
from gi.overrides.keysyms import careof


class TiledMap(object):
    '''
    A tiled map system running under pi3d.  Organises and draws small parts of a map (tiles) from a larger collection. 
    '''


    def __init__(self, tileSize, w, h, z, tileResolution=1, alpha = 1.0):
        '''
        tileSize = size of tile in meters
        tileResolution = pixels per meter (not supported yet)
        w = Pixel height to show
        h = Pixel width to show
        z = Map stack depth
        alpha = transparency
        '''        
        from pi3d.Display import Display
        
        self.tileSize = tileSize
        self.tileResolution = tileResolution
        self.tile_scale = 1.0 / (self.tileSize * self.tileResolution)

        self.w = w
        self.h = h
        self.z = z
        self.alpha = alpha

        self.zoom = 1.0
        
        self.tiles = dict()
        self.map_objects = list()
   
        # Map focus point    
        self._map_focus         = CoordSys.Cartesian(0.0,0.0)
        
        self._aircraft_pos      = CoordSys.Cartesian(0.0,0.0)
        self._last_aircraft_pos = CoordSys.Cartesian(0.0,0.0)
        
        self._climbrate = 0.0
        
        self.home_colour = (0,0,1.0,0.5)
        
        # camera for viewing the map. Owned by the track since it can move
        self.map_camera = pi3d.Camera(is_3d = False)

        # 2d camera for drawing on the map tiles
        self.tile_camera = pi3d.Camera(is_3d=False)    #pi3d.Camera(is_3d=False)

        # shader for drawing the map        
        self.flatsh = pi3d.Shader("uv_flat")
        
        #shader for drawing simple colour shapes on the map
        self.matsh = pi3d.Shader("mat_flat")
        
        self.screen_width = Display.INSTANCE.width
        self.screen_height = Display.INSTANCE.height
        
        self.cam_xoffset = (self.screen_width-tileSize) * 0.5
        self.cam_yoffset = (self.screen_height-tileSize) * 0.5
  
        self.tiles = dict()
                
        self.inits_done = 0
        
    
    def get_tile_display_range(self):
        tiles_x = int(ceil(self.w/self.tileSize) / self.zoom)
        tiles_y = int(ceil(self.h/self.tileSize) / self.zoom)
        
        tileCoord = CoordSys.TileCoord(cartesian=self._map_focus, tileSize=self.tileSize)
        tileNum = tileCoord.get_tile_number()
                                       
        x_span = (tiles_x/2)+1
        y_span = (tiles_y/2)+1
        
        x0 = tileNum.tile_num_x
        y0 = tileNum.tile_num_y
#        return 0,0,0,0
        return x0-x_span, y0-y_span, x0+x_span,  y0+y_span
    
    # set the map focus in relative position from origin [x, y]
    def set_map_focus(self, pos):
        self._map_focus = pos
        
    def set_map_origin(self, origin):
        self._origin = origin

    def set_aircraft_pos(self, pos):
        self._aircraft_pos = pos
        
    def set_climbrate(self, climbrate):
        self._climbrate = climbrate
            
        
    def gen_map(self):
        if self.inits_done == 0:
            self.tile_camera.reset(is_3d=False)
            self.tile_camera.position((self.cam_xoffset,self.cam_yoffset,0))
            self.inits_done = 1
            return
        
        self.update_tiles()
        
        for tile in self.tiles.itervalues():
            if tile.is_draw_done() and tile.updateCount == 0:
                if tile.tile_no.tile_num_x == 0 and tile.tile_no.tile_num_y == 0:
                    tile.texture._start(True)
                    self.draw_home()
                    tile.texture._end()
                else:
                    tile.texture._start(True)
                    self.draw_tile_centre()
                    tile.texture._end()

                tile.updateCount = 1


    def update_tiles(self):
        x0, y0, x1, y1 = self.get_tile_display_range()
        for x in range(x0,x1+0):
            for y in range(y0,y1+0):
                key = '{:d},{:d}'.format(x , y)
                if not self.tiles.has_key(key):
                    new_tile = MapTile(map_camera=self.map_camera, map_shader = self.flatsh, tilePixels=self.tileSize, tile_x=x, tile_y=y)
                    self.tiles[key] = new_tile
                    
    def interpolate_ratio(self, ratio, y1, y2):
        delta = y2 - y1
        return (ratio * delta) + y1
        
    def interpolate(self, x, x1, x2, y1, y2):
        deltax = x2 - x1
        ratio = (x - x1) / deltax
        deltay = y2 - y1
        return (ratio * deltay) + y1
                    
    def get_rate_colour(self, rate):
        colour = (1.0, 1.0, 1.0, 1.0)

        if(rate <= -20):
            colour = (0.0, 0.0, 1.0)
        elif(rate >= 20):
            colour = (1.0, 0.0, 0.0)
        else:
            hue = 0.333
            if rate >= 0:
                hue = self.interpolate(rate, 0, 20, 0.333, 0.0)
            else:
                hue = self.interpolate(rate, 0, -20, 0.666, 0.0)
            colour = colorsys.hsv_to_rgb(hue, 1.0, 1.0)
        return colour

    def set_tile_alpha(self, tile):
        tile.set_alpha(self.alpha)
                    
    def add_segment(self):
        if self._last_aircraft_pos == self._aircraft_pos:
            return
        
        pos1 = CoordSys.TileCoord(cartesian=self._last_aircraft_pos, tileSize=self.tileSize)
        pos2 = CoordSys.TileCoord(cartesian=self._aircraft_pos, tileSize=self.tileSize)
        rate_colour = self.get_rate_colour(self._climbrate)
        
        tile1 = pos1.get_tile_number()
        tile2 = pos2.get_tile_number()
        
        for x in range (tile1.tile_num_x, tile2.tile_num_x+1):
            for y in range (tile1.tile_num_y, tile2.tile_num_y+1):
                key = '{:d},{:d}'.format(x , y)
                if self.tiles.has_key(key):
                    tile = self.tiles[key]
                    rel1 = pos1.get_relative_tile_coord(tile.tile_no)
                    rel2 = pos2.get_relative_tile_coord(tile.tile_no)
                    pix1 = rel1.get_tile_pixel_pos(tile.tilePixels).point()
                    pix2 = rel2.get_tile_pixel_pos(tile.tilePixels).point()
                    
                    tile.start(False)
                    segment = Line2d(camera=self.tile_camera, matsh=self.matsh, points=(pix1,pix2), thickness=3, colour=rate_colour )
                    segment.draw()              
                    tile.end()
        self._last_aircraft_pos = self._aircraft_pos


    def draw(self):
        camera = self.map_camera
        camera.reset(is_3d=False, scale=self.zoom)
        tileCoord = CoordSys.TileCoord(cartesian=self._map_focus, tileSize=self.tileSize)
        pxlPos = tileCoord.get_abs_pixel_pos(self.tileSize)
        camera.position((pxlPos.map_pixel_x,pxlPos.map_pixel_y, 0.0))

        x0, y0, x1, y1 = self.get_tile_display_range()
        for x in range(x0,x1):
            for y in range(y0,y1):
                key = '{:d},{:d}'.format(x , y)
                if self.tiles.has_key(key):
                    tile = self.tiles[key]
                    self.set_tile_alpha(tile)
                    tile.draw()
                    

    def draw_home(self):
#        bar_shape = pi3d.Plane(camera=self.camera2d,  w=100, h=100)
        bar_shape = pi3d.Plane(camera=self.tile_camera,  w=20, h=20)
        bar_shape.set_draw_details(self.matsh, [], 0, 0)
        bar_shape.set_material(self.home_colour)
        bar_shape.position( 0,  0, 5)
        bar_shape.draw()
        
    def draw_tile_centre(self):
        bar_shape = pi3d.Plane(camera=self.tile_camera,  w=3, h=20)
        bar_shape.set_draw_details(self.matsh, [], 0, 0)
        bar_shape.set_material(self.home_colour)
        bar_shape.position( 0,  0, 5)
        bar_shape.draw()

        bar_shape = pi3d.Plane(camera=self.tile_camera,  w=20, h=3)
        bar_shape.set_draw_details(self.matsh, [], 0, 0)
        bar_shape.set_material(self.home_colour)
        bar_shape.position( 0,  0, 5)
        bar_shape.draw()

#        bar_shape.set_material((1.0, 0, 0))
#        bar_shape.position( -self.tileSize*0.25,  -self.tileSize*0.25, 5)
#        bar_shape.draw()

#        bar_shape.set_material((0, 1.0, 0))
#        bar_shape.position( self.tileSize*0.25,  self.tileSize*0.25, 5)
#        bar_shape.draw()
    