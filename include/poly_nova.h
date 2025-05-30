// PolyNova3D (version 3.3)
/***************************************************************
 Copyright (C) 1999 Novasoft Consulting

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Library General Public
 License as published by the Free Software Foundation; either
 version 2 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Library General Public License for more details.

 You should have received a copy of the GNU Library General Public
 License along with this library; if not, write to the Free
 Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *****************************************************************/
#ifndef __POLY_NOVA_H
#define __POLY_NOVA_H

// Standard includes.
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <vector>

// SDL stuff.
#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>

// Used by all game states.
class GameState;
#include <display_manager.h>
#include <audio_manager.h>
#include <input_manager.h>
#include <yamlish.h>
#include <nova_utils.h>
#include <logging.h>
#include <nova_color.h>
#include <nova_vertex.h>
#include <nova_matrix.h>
#include <render_vertex.h>
#include <display_vertex.h>
#include <nova_line.h>
#include <highlight_line.h>
#include <matrix_object.h>
#include <camera.h>
#include <sprite_definition.h>
#include <sprite_definition_library.h>
#include <sprite.h>
#include <character_sprite.h>
#include <character_digit.h>
#include <high_score_handler.h>
#include <game_state.h>

// Play state objects.
class PlayState;
#include <play/grid_line.h>
#include <play/play_area_grid.h>
#include <play/play_area_border.h>
#include <play/star.h>
#include <play/missile.h>
#include <play/alien_missile.h>
#include <play/player_missile.h>
#include <play/laser.h>
#include <play/alien.h>
#include <play/nugget.h>
#include <play/multiplier_nugget.h>
#include <play/power_up_nugget.h>
#include <play/bounce_alien.h>
#include <play/bounce_cube.h>
#include <play/bounce_wanderer.h>
#include <play/bounce_star.h>
#include <play/bounce_hexagon.h>
#include <play/chase_alien.h>
#include <play/chase_rhombus.h>
#include <play/chase_star.h>
#include <play/chase_shuriken.h>
#include <play/chase_concave.h>
#include <play/attack_tank.h>
#include <play/attack_ship.h>
#include <play/attack_artillery.h>
#include <play/attack_neutron.h>
#include <play/rocket_ship.h>
#include <play/flying_saucer.h>
#include <play/alien_shield.h>
#include <play/player_clone.h>
#include <play/black_hole.h>
#include <play/mini_gate.h>
#include <play/snake_segment.h>
#include <play/snake_head_segment.h>
#include <play/snake_body_segment.h>
#include <play/snake_tail_segment.h>
#include <play/snake.h>
#include <play/player_shield.h>
#include <play/player.h>
#include <play/hud_panel.h>
#include <play/game_over_panel.h>
#include <play/current_score_panel.h>
#include <play/high_score_panel.h>
#include <play/player_stats_panel.h>
#include <play/nova_particle.h>
#include <play/explosion.h>
#include <play/play_area_controller.h>
#include <play/starfield_controller.h>
#include <play/alien_controller.h>
#include <play/nugget_controller.h>
#include <play/missile_controller.h>
#include <play/explosion_controller.h>
#include <play/hud_controller.h>
#include <play/play_state.h>

// Menu state objects.
class MenuState;
#include <menu/menu_background.h>
#include <menu/label.h>
#include <menu/menu_panel.h>
#include <menu/menu_title.h>
#include <menu/control.h>
#include <menu/button.h>
#include <menu/option_toggle.h>
#include <menu/option_selector.h>
#include <menu/option_text_edit.h>
#include <menu/high_score_entry.h>
#include <menu/base_menu.h>
#include <menu/main_menu.h>
#include <menu/high_score_menu.h>
#include <menu/options_menu.h>
#include <menu/menu_state.h>

// Global pointer.
#include <world_manager.h>
extern WorldManager* g_worldManager;

#endif // __POLY_NOVA_H

