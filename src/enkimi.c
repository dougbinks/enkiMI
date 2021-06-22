
/*
 * Copyright (c) 2017 Juliette Foucaut & Doug Binks
 * 
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * 
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgement in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include "enkimi.h"

// Add allocation pAllocation_ to stream, which will be freed using enkiNBTFreeAllocations
void enkiNBTAddAllocation( enkiNBTDataStream* pStream_, void* pAllocation_ );

static const uint32_t SECTOR_SIZE = 4096;

static const char* tagIdString[] =
{
	"TAG_End",
	"TAG_Byte",
	"TAG_Short",
	"TAG_Int",
	"TAG_Long",
	"TAG_Float",
	"TAG_Double",
	"TAG_Byte_Array",
	"TAG_String",
	"TAG_List",
	"TAG_Compound",
	"TAG_Int_Array",
	"TAG_Long_Array",
};

static uint32_t minecraftPalette[] = 
{ 
	0xff000000, 0xff7d7d7d, 0xff4cb376, 0xff436086, 0xff7a7a7a, 0xff4e7f9c, 0xff256647, 0xff535353, 0xffdcaf70, 0xffdcaf70, 
	0xff135bcf, 0xff125ad4, 0xffa0d3db, 0xff7a7c7e, 0xff7c8b8f, 0xff7e8287, 0xff737373, 0xff315166, 0xff31b245, 0xff54c3c2, 
	0xfff4f0da, 0xff867066, 0xff894326, 0xff838383, 0xff9fd3dc, 0xff324364, 0xff3634b4, 0xff23c7f6, 0xff7c7c7c, 0xff77bf8e, 
	0xffdcdcdc, 0xff296595, 0xff194f7b, 0xff538ba5, 0xff5e96bd, 0xffdddddd, 0xffe5e5e5, 0xff00ffff, 0xff0d00da, 0xff415778, 
	0xff0d0fe1, 0xff4eecf9, 0xffdbdbdb, 0xffa1a1a1, 0xffa6a6a6, 0xff0630bc, 0xff0026af, 0xff39586b, 0xff658765, 0xff1d1214, 
	0xff00ffff, 0xff005fde, 0xff31271a, 0xff4e87a6, 0xff2a74a4, 0xff0000ff, 0xff8f8c81, 0xffd5db61, 0xff2e5088, 0xff17593c, 
	0xff335682, 0xff676767, 0xff00b9ff, 0xff5b9ab8, 0xff387394, 0xff345f79, 0xff5190b6, 0xff6a6a6a, 0xff5b9ab8, 0xff40596a, 
	0xff7a7a7a, 0xffc2c2c2, 0xff65a0c9, 0xff6b6b84, 0xff2d2ddd, 0xff000066, 0xff0061ff, 0xff848484, 0xfff1f1df, 0xffffad7d, 
	0xfffbfbef, 0xff1d830f, 0xffb0a49e, 0xff65c094, 0xff3b5985, 0xff42748d, 0xff1b8ce3, 0xff34366f, 0xff334054, 0xff45768f, 
	0xffbf0a57, 0xff2198f1, 0xffffffec, 0xffb2b2b2, 0xffb2b2b2, 0xffffffff, 0xff2d5d7e, 0xff7c7c7c, 0xff7a7a7a, 0xff7cafcf, 
	0xff78aaca, 0xff6a6c6d, 0xfff4efd3, 0xff28bdc4, 0xff69dd92, 0xff53ae73, 0xff0c5120, 0xff5287a5, 0xff2a4094, 0xff7a7a7a, 
	0xff75718a, 0xff767676, 0xff1a162c, 0xff1a162c, 0xff1a162c, 0xff2d28a6, 0xffb1c454, 0xff51677c, 0xff494949, 0xff343434, 
	0xffd18934, 0xffa5dfdd, 0xff0f090c, 0xff316397, 0xff42a0e3, 0xff4d84a1, 0xff49859e, 0xff1f71dd, 0xffa8e2e7, 0xff74806d, 
	0xff3c3a2a, 0xff7c7c7c, 0xff5a5a5a, 0xff75d951, 0xff345e81, 0xff84c0ce, 0xff455f88, 0xff868b8e, 0xffd7dd74, 0xff595959, 
	0xff334176, 0xff008c0a, 0xff17a404, 0xff5992b3, 0xffb0b0b0, 0xff434347, 0xff1d6b9e, 0xff70fdfe, 0xffe5e5e5, 0xff4c4a4b, 
	0xffbdc6bf, 0xffddedfb, 0xff091bab, 0xff4f547d, 0xff717171, 0xffdfe6ea, 0xffe3e8eb, 0xff41819b, 0xff747474, 0xffa1b2d1, 
	0xfff6f6f6, 0xff878787, 0xff395ab0, 0xff325cac, 0xff152c47, 0xff65c878, 0xff3534df, 0xffc7c7c7, 0xffa5af72, 0xffbec7ac, 
	0xff9fd3dc, 0xffcacaca, 0xff425c96, 0xff121212, 0xfff4bfa2, 0xff1474cf, 0xfff0f0f0, 0xfff0f0f0, 0xfff0f0f0, 0xff1d56ac, 
	0xff1d57ae, 0xff1d57ae, 0xff1d57ae, 0xff243c50, 0xff8dcddd, 0xff4d7aaf, 0xff0e2034, 0xff366bcf, 0xff355d7e, 0xff7bb8c7, 
	0xff5f86bb, 0xff1e2e3f, 0xff3a6bc5, 0xff30536e, 0xffe0f3f7, 0xff5077a9, 0xff2955aa, 0xff21374e, 0xffcdc5dc, 0xff603b60, 
	0xff856785, 0xffa679a6, 0xffaa7eaa, 0xffa879a8, 0xffa879a8, 0xffa879a8, 0xffaae6e1, 0xffaae6e1, 0xff457d98, 0xfff0f0f0, 
	0xfff0f0f0, 0xfff0f0f0, 0xfff0f0f0, 0xfff0f0f0, 0xfff0f0f0, 0xfff0f0f0, 0xfff0f0f0, 0xfff0f0f0, 0xfff0f0f0, 0xfff0f0f0, 
	0xfff0f0f0, 0xfff0f0f0, 0xfff0f0f0, 0xfff0f0f0, 0xfff0f0f0, 0xfff0f0f0, 0xfff0f0f0, 0xfff0f0f0, 0xfff0f0f0, 0xfff0f0f0, 
	0xfff0f0f0, 0xfff0f0f0, 0xfff0f0f0, 0xfff0f0f0, 0xfff0f0f0, 0xfff0f0f0, 0xfff0f0f0, 0xfff0f0f0, 0xfff0f0f0, 0xfff0f0f0, 
	0xfff0f0f0, 0xfff0f0f0, 0xfff0f0f0, 0xfff0f0f0, 0xfff0f0f0, 0xfff0f0f0, 0xfff0f0f0, 0xfff0f0f0, 0xfff0f0f0, 0xfff0f0f0, 
	0xfff0f0f0, 0xfff0f0f0, 0xfff0f0f0, 0xfff0f0f0, 0xfff0f0f0, 0xff242132 };

// BlockIds from: https://minecraft.fandom.com/wiki/Java_Edition_data_values/Pre-flattening/Block_IDs
// Then get dataValue from each material's page: https://minecraft.fandom.com/wiki/Stone#Data_values
static enkiMINamespaceAndBlockID defaultNamespaceAndBlockIDs[] = 
{
	{ "minecraft:air",                                  0,   0   }, 
	{ "minecraft:cave_air",                             0,   1   }, // Added after flattening, data value not correct
	{ "minecraft:void_air",                             0,   2   }, // Added after flattening, data value not correct
	{ "minecraft:stone",                                1,   0   }, 
	{ "minecraft:granite",                              1,   1   }, 
	{ "minecraft:polished_granite",                     1,   2   },
	{ "minecraft:diorite",                              1,   3   }, 
	{ "minecraft:polished_diorite",                     1,   4   },
	{ "minecraft:andesite",                             1,   5   }, 
	{ "minecraft:polished_andesite",                    1,   6   },
	{ "minecraft:grass_block",                          2,   0   }, // https://minecraft.fandom.com/wiki/Grass_Block
	{ "minecraft:dirt",                                 3,   0   }, 
	{ "minecraft:coarse_dirt",                          3,   1   }, 
	{ "minecraft:cobblestone",                          4,   0   },
	{ "minecraft:oak_planks",                           5,   0   },
	{ "minecraft:spruce_planks",                        5,   1   },
	{ "minecraft:birch_planks",                         5,   2   },
	{ "minecraft:jungle_planks",                        5,   3   },
	{ "minecraft:acacia_planks",                        5,   4   },
	{ "minecraft:dark_oak_planks",                      5,   5   },
	{ "minecraft:crimson_planks",                       5,   6   },
	{ "minecraft:warped_planks",                        5,   7   },
	{ "minecraft:oak_sapling",                          6,   0   },
	{ "minecraft:spruce_sapling",                       6,   1   },
	{ "minecraft:birch_sapling",                        6,   2   },
	{ "minecraft:jungle_sapling",                       6,   3   },
	{ "minecraft:acacia_sapling",                       6,   4   },
	{ "minecraft:dark_oak_sapling",                     6,   5   },
	{ "minecraft:bedrock",                              7,   0   },
	{ "minecraft:flowing_water",                        8,   0   },
	{ "minecraft:water",                                9,   0   },
	{ "minecraft:flowing_lava",                         10,  0   },
	{ "minecraft:lava",                                 11,  0   },
	{ "minecraft:sand",                                 12,  0   },
	{ "minecraft:red_sand",                             12,  1   },
	{ "minecraft:gravel",                               13,  0   },
	{ "minecraft:gold_ore",                             14,  0   },
	{ "minecraft:deepslate_gold_ore",                   14,  1   },
	{ "minecraft:iron_ore",                             15,  0   },
	{ "minecraft:deepslate_iron_ore",                   15,  1   },
	{ "minecraft:coal_ore",                             16,  0   },
	{ "minecraft:deepslate_coal_ore",                   16,  1   },
	{ "minecraft:oak_log",                              17,  0   }, // up-down
	{ "minecraft:spruce_log",                           17,  1   }, // up-down
	{ "minecraft:birch_log",                            17,  2   }, // up-down
	{ "minecraft:jungle_log",                           17,  3   }, // up-down
	{ "minecraft:oak_log",                              17,  4   }, // east-west
	{ "minecraft:spruce_log",                           17,  5   }, // east-west
	{ "minecraft:birch_log",                            17,  6   }, // east-west
	{ "minecraft:jungle_log",                           17,  7   }, // east-west
	{ "minecraft:oak_log",                              17,  8   }, // north-south
	{ "minecraft:spruce_log",                           17,  9   }, // north-south
	{ "minecraft:birch_log",                            17,  10  }, // north-south
	{ "minecraft:jungle_log",                           17,  11  }, // north-south 
	{ "minecraft:oak_leaves",                           18,  0   }, 
	{ "minecraft:spruce_leaves",                        18,  1   }, 
	{ "minecraft:birch_leaves",                         18,  2   }, 
	{ "minecraft:jungle_leaves",                        18,  3   }, 
	{ "minecraft:oak_leaves",                           18,  4   }, // check for decay (if no log within 4 blocks decay)
	{ "minecraft:spruce_leaves",                        18,  5   }, // check for decay (if no log within 4 blocks decay)
	{ "minecraft:birch_leaves",                         18,  6   }, // check for decay (if no log within 4 blocks decay)
	{ "minecraft:jungle_leaves",                        18,  7   }, // check for decay (if no log within 4 blocks decay)
	{ "minecraft:oak_leaves",                           18,  8   }, // persistent
	{ "minecraft:spruce_leaves",                        18,  9   }, // persistent
	{ "minecraft:birch_leaves",                         18,  10  }, // persistent
	{ "minecraft:jungle_leaves",                        18,  11  }, // persistent
	{ "minecraft:oak_leaves",                           18,  12  }, // persistent (12-15 same as 8-11)
	{ "minecraft:spruce_leaves",                        18,  13  }, // persistent (12-15 same as 8-11)
	{ "minecraft:birch_leaves",                         18,  14  }, // persistent (12-15 same as 8-11)
	{ "minecraft:jungle_leaves",                        18,  15  }, // persistent (12-15 same as 8-11)
	{ "minecraft:sponge",                               19,  0   }, 
	{ "minecraft:wet_sponge",                           19,  1   }, 
	{ "minecraft:glass",                                20,  0   }, 
	{ "minecraft:lapis_ore",                            21,  0   }, 
	{ "minecraft:deepslate_lapis_ore",                  21,  1   },  // Added after flattening, data value not correct
	{ "minecraft:lapis_block",                          22,  0   }, 
	{ "minecraft:dispenser",                            23,  0   }, 
	{ "minecraft:sandstone",                            24,  0   },
	{ "minecraft:chiseled_sandstone",                   24,  1   },
	{ "minecraft:cut_sandstone",                        24,  2   },
	{ "minecraft:smooth_sandstone",                     24,  3   },
	{ "minecraft:note_block",                           25,  0   }, 
	{ "minecraft:white_bed",                            26,  0   }, 
	{ "minecraft:orange_bed",                           26,  1   }, 
	{ "minecraft:magenta_bed",                          26,  2   }, 
	{ "minecraft:light_blue_bed",                       26,  3   }, 
	{ "minecraft:yellow_bed",                           26,  4   }, 
	{ "minecraft:lime_bed",                             26,  5   }, 
	{ "minecraft:pink_bed",                             26,  6   }, 
	{ "minecraft:gray_bed",                             26,  7   }, 
	{ "minecraft:light_gray_bed",                       26,  8   }, 
	{ "minecraft:cyan_bed",                             26,  9   }, 
	{ "minecraft:purple_bed",                           26,  10  }, 
	{ "minecraft:blue_bed",                             26,  11  }, 
	{ "minecraft:brown_bed",                            26,  12  }, 
	{ "minecraft:green_bed",                            26,  13  }, 
	{ "minecraft:red_bed",                              26,  14  }, 
	{ "minecraft:black_bed",                            26,  15  }, 
	{ "minecraft:powered_rail",                         27,  0   }, 
	{ "minecraft:detector_rail",                        28,  0   },
	{ "minecraft:sticky_piston",                        29,  0   }, // facing down
	{ "minecraft:sticky_piston",                        29,  1   }, // facing up
	{ "minecraft:sticky_piston",                        29,  2   }, // facing north
	{ "minecraft:sticky_piston",                        29,  3   }, // facing south
	{ "minecraft:sticky_piston",                        29,  4   }, // facing west
	{ "minecraft:sticky_piston",                        29,  5   }, // facing east
	{ "minecraft:cobweb",                               30,  0   },
	{ "minecraft:grass",                                31,  0   }, // short plants, https://minecraft.fandom.com/wiki/Grass
	{ "minecraft:fern",                                 31,  1   }, // short plants, 
	{ "minecraft:seagrass",                             31,  2   }, // short plants, - Added after flattening, data value not correct
	{ "minecraft:dead_bush",                            32,  0   }, 
	{ "minecraft:piston",                               33,  0   }, // facing down
	{ "minecraft:piston",                               33,  1   }, // facing up
	{ "minecraft:piston",                               33,  2   }, // facing north
	{ "minecraft:piston",                               33,  3   }, // facing south
	{ "minecraft:piston",                               33,  4   }, // facing west
	{ "minecraft:piston",                               33,  5   }, // facing east
	{ "minecraft:piston_head",                          34,  0   }, // facing down
	{ "minecraft:piston_head",                          34,  1   }, // facing up
	{ "minecraft:piston_head",                          34,  2   }, // facing north
	{ "minecraft:piston_head",                          34,  3   }, // facing south
	{ "minecraft:piston_head",                          34,  4   }, // facing west
	{ "minecraft:piston_head",                          34,  5   }, // facing east
	{ "minecraft:white_wool",                           35,  0   }, 
	{ "minecraft:orange_wool",                          35,  1   }, 
	{ "minecraft:magenta_wool",                         35,  2   }, 
	{ "minecraft:light_blue_wool",                      35,  3   }, 
	{ "minecraft:yellow_wool",                          35,  4   }, 
	{ "minecraft:lime_wool",                            35,  5   }, 
	{ "minecraft:pink_wool",                            35,  6   }, 
	{ "minecraft:gray_wool",                            35,  7   }, 
	{ "minecraft:light_gray_wool",                      35,  8   }, 
	{ "minecraft:cyan_wool",                            35,  9   }, 
	{ "minecraft:purple_wool",                          35,  10  }, 
	{ "minecraft:black_wool",                           35,  15  }, 
	{ "minecraft:blue_wool",                            35,  11  }, 
	{ "minecraft:brown_wool",                           35,  12  }, 
	{ "minecraft:green_wool",                           35,  13  }, 
	{ "minecraft:red_wool",                             35,  14  },
	{ "minecraft:moving_piston",                        36,  0   }, // facing down
	{ "minecraft:moving_piston",                        36,  1   }, // facing up
	{ "minecraft:moving_piston",                        36,  2   }, // facing north
	{ "minecraft:moving_piston",                        36,  3   }, // facing south
	{ "minecraft:moving_piston",                        36,  4   }, // facing west
	{ "minecraft:moving_piston",                        36,  5   }, // facing east
	{ "minecraft:dandelion",                            37,  0   }, 
	{ "minecraft:poppy",                                38,  0   }, // small flowers
	{ "minecraft:blue_orchid",                          38,  1   }, // small flowers
	{ "minecraft:Allium",                               38,  2   }, // small flowers
	{ "minecraft:azure_bluet",                          38,  3   }, // small flowers
	{ "minecraft:red_tulip",                            38,  4   }, // small flowers
	{ "minecraft:orange_tulip",                         38,  5   }, // small flowers
	{ "minecraft:white_tulip",                          38,  6   }, // small flowers
	{ "minecraft:pink_tulip",                           38,  7   }, // small flowers
	{ "minecraft:oxeye_daisy",                          38,  8   }, // small flowers
	{ "minecraft:cornflower",                           38,  9   }, // small flowers
	{ "minecraft:lily_of_the_valley",                   38,  10  }, // small flowers
	{ "minecraft:wither_rose",                          38,  11  }, // small flowers
	{ "minecraft:brown_mushroom",                       39,  0   },
	{ "minecraft:red_mushroom",                         40,  0   },
	{ "minecraft:gold_block",                           41,  0   },
	{ "minecraft:iron_block",                           42,  0   },
	{ "minecraft:smooth_stone_slab",                    43,  0   }, 
	{ "minecraft:stone_slab",                           44,  0   },
	{ "minecraft:bricks",                               45,  0   },
	{ "minecraft:tnt",                                  46,  0   },
	{ "minecraft:bookshelf",                            47,  0   },
	{ "minecraft:mossy_cobblestone",                    48,  0   },
	{ "minecraft:obsidian",                             49,  0   },
	{ "minecraft:torch",                                50,  0   },
	{ "minecraft:wall_torch",                           50,  1   }, // facing east
	{ "minecraft:wall_torch",                           50,  2   }, // facing west
	{ "minecraft:wall_torch",                           50,  3   }, // facing south
	{ "minecraft:wall_torch",                           50,  4   }, // facing north
	{ "minecraft:wall_torch",                           50,  5   }, // facing up
	{ "minecraft:soul_torch",                           50,  6   },
	{ "minecraft:soul_wall_torch",                      50,  7   }, // facing east - Added after flattening, data value not correct
	{ "minecraft:soul_wall_torch",                      50,  8   }, // facing west - Added after flattening, data value not correct
	{ "minecraft:soul_wall_torch",                      50,  9   }, // facing south - Added after flattening, data value not correct
	{ "minecraft:soul_wall_torch",                      50,  10  }, // facing north - Added after flattening, data value not correct
	{ "minecraft:soul_wall_torch",                      50,  11  }, // facing up  - Added after flattening, data value not correct
	{ "minecraft:fire",                                 51,  0   },
	{ "minecraft:soul_fire",                            51,  1   }, // Added after flattening, data value not correct
	{ "minecraft:spawner",                              52,  0   }, 
	{ "minecraft:mob_spawner",                          52,  1   },
	{ "minecraft:oak_stairs",                           53,  0   }, // facing east 
	{ "minecraft:oak_stairs",                           53,  1   }, // facing north 
	{ "minecraft:oak_stairs",                           53,  2   }, // facing south 
	{ "minecraft:oak_stairs",                           53,  3   }, // facing west 
	{ "minecraft:chest",                                54,  0   }, 
	{ "minecraft:redstone_wire",                        55,  0   }, 
	{ "minecraft:redstone",                             55,  1   }, 
	{ "minecraft:diamond_ore",                          56,  0   }, 
	{ "minecraft:deepslate_diamond_ore",                56,  1   }, 
	{ "minecraft:diamond_block",                        57,  0   }, 
	{ "minecraft:crafting_table",                       58,  0   },      
	{ "minecraft:wheat",                                59,  0   }, // https://minecraft.fandom.com/wiki/Wheat_Seeds
	{ "minecraft:wheat",                                59,  1   }, // growth
	{ "minecraft:wheat",                                59,  2   }, // growth 
	{ "minecraft:wheat",                                59,  3   }, // growth 
	{ "minecraft:wheat",                                59,  4   }, // growth 
	{ "minecraft:wheat",                                59,  5   }, // growth 
	{ "minecraft:wheat",                                59,  6   }, // growth 
	{ "minecraft:wheat",                                59,  7   }, // growth 
	{ "minecraft:farmland",                             60,  0   }, 
	{ "minecraft:furnace",                              61,  0   }, // facing east
	{ "minecraft:furnace",                              61,  1   }, // facing north
	{ "minecraft:furnace",                              61,  2   }, // facing south
	{ "minecraft:furnace",                              61,  3   }, // facing west
    { "minecraft:lit_furnace",                          62,  0   }, // facing east
	{ "minecraft:lit_furnace",                          62,  1   }, // facing north
	{ "minecraft:lit_furnace",                          62,  2   }, // facing south
	{ "minecraft:lit_furnace",                          62,  3   }, // facing west
	{ "minecraft:oak_sign",                             63,  0   },
	{ "minecraft:spruce_sign",                          63,  1   },
	{ "minecraft:birch_sign",                           63,  2   },
	{ "minecraft:jungle_sign",                          63,  3   },
	{ "minecraft:acacia_sign",                          63,  4   },
	{ "minecraft:dark_oak_sign",                        63,  5   },
	{ "minecraft:crimson_sign",                         63,  6   },
	{ "minecraft:warped_sign",                          63,  7   },
    { "minecraft:oak_door",                             64,  0   }, // facing east
	{ "minecraft:oak_door",                             64,  1   }, // facing north
	{ "minecraft:oak_door",                             64,  2   }, // facing south
	{ "minecraft:oak_door",                             64,  3   }, // facing west
    { "minecraft:ladder",                               65,  0   }, // facing east
	{ "minecraft:ladder",                               65,  1   }, // facing north
	{ "minecraft:ladder",                               65,  2   }, // facing south
	{ "minecraft:ladder",                               65,  3   }, // facing west
    { "minecraft:rail",                                 66,  0   }, // east-west
	{ "minecraft:rail",                                 66,  1   }, // north-south
	{ "minecraft:cobblestone_stairs",                   67,  0   }, // facing east 
	{ "minecraft:cobblestone_stairs",                   67,  1   }, // facing north 
	{ "minecraft:cobblestone_stairs",                   67,  2   }, // facing south 
	{ "minecraft:cobblestone_stairs",                   67,  3   }, // facing west 
	{ "minecraft:oak_wall_sign",                        68,  0   }, // facing east
	{ "minecraft:oak_wall_sign",                        68,  1   }, // facing north
	{ "minecraft:oak_wall_sign",                        68,  2   }, // facing south
	{ "minecraft:oak_wall_sign",                        68,  3   }, // facing west
    { "minecraft:spruce_wall_sign",                     68,  4   }, // facing east
    { "minecraft:spruce_wall_sign",                     68,  5   }, // facing north
    { "minecraft:spruce_wall_sign",                     68,  6   }, // facing south
    { "minecraft:spruce_wall_sign",                     68,  7   }, // facing west
	{ "minecraft:birch_wall_sign",                      68,  8   }, // facing east
	{ "minecraft:birch_wall_sign",                      68,  9   }, // facing north
	{ "minecraft:birch_wall_sign",                      68,  10  }, // facing south
	{ "minecraft:birch_wall_sign",                      68,  11  }, // facing west
	{ "minecraft:jungle_wall_sign",                     68,  12  }, // facing east
	{ "minecraft:jungle_wall_sign",                     68,  13  }, // facing north
	{ "minecraft:jungle_wall_sign",                     68,  14  }, // facing south
	{ "minecraft:jungle_wall_sign",                     68,  15  }, // facing west
	{ "minecraft:acacia_wall_sign",                     68,  16  }, // facing east
	{ "minecraft:acacia_wall_sign",                     68,  17  }, // facing north
	{ "minecraft:acacia_wall_sign",                     68,  18  }, // facing south
	{ "minecraft:acacia_wall_sign",                     68,  19  }, // facing west
    { "minecraft:dark_oak_wall_sign",                   68,  20  }, // facing east
	{ "minecraft:dark_oak_wall_sign",                   68,  21  }, // facing north
	{ "minecraft:dark_oak_wall_sign",                   68,  22  }, // facing south
	{ "minecraft:dark_oak_wall_sign",                   68,  23  }, // facing west
    { "minecraft:crimson_wall_sign",                    68,  24  }, // facing east
	{ "minecraft:crimson_wall_sign",                    68,  25  }, // facing north
	{ "minecraft:crimson_wall_sign",                    68,  26  }, // facing south
	{ "minecraft:crimson_wall_sign",                    68,  27  }, // facing west
    { "minecraft:warped_wall_sign",                     68,  28  }, // facing east
	{ "minecraft:warped_wall_sign",                     68,  29  }, // facing north
	{ "minecraft:warped_wall_sign",                     68,  30  }, // facing south
	{ "minecraft:warped_wall_sign",                     68,  31  }, // facing west
	{ "minecraft:lever",                                69,  0   }, 
	{ "minecraft:stone_pressure_plate",                 70,  0   },
	{ "minecraft:polished_blackstone_pressure_plate",   70,  0   },
    { "minecraft:iron_door",                            71,  0   }, // facing east
	{ "minecraft:iron_door",                            71,  1   }, // facing north
	{ "minecraft:iron_door",                            71,  2   }, // facing south
	{ "minecraft:iron_door",                            71,  3   }, // facing west
	{ "minecraft:oak_pressure_plate",                   72,  0   }, 
	{ "minecraft:spruce_pressure_plate",                72,  1   }, 
	{ "minecraft:birch_pressure_plate",                 72,  2   }, 
	{ "minecraft:jungle_pressure_plate",                72,  3   }, 
	{ "minecraft:acacia_pressure_plate",                72,  4   }, 
	{ "minecraft:dark_oak_pressure_plate",              72,  5   }, 
	{ "minecraft:crimson_pressure_plate",               72,  6   }, 
	{ "minecraft:warped_pressure_plate",                72,  7   }, 
	{ "minecraft:redstone_ore",                         73,  0   }, 
	{ "minecraft:deepslate_redstone_ore",               73,  1   }, 
	{ "minecraft:lit_redstone_ore",                     74,  0   }, 
	{ "minecraft:lit_deepslate_redstone_ore",           74,  1   }, 
	{ "minecraft:redstone_torch",                       75,  0   }, // unlit
	{ "minecraft:redstone_torch",                       75,  1   }, // lit
	{ "minecraft:redstone_wall_torch",                  76,  0   }, // unlit - facing east
	{ "minecraft:redstone_wall_torch",                  76,  1   }, // unlit - facing west
	{ "minecraft:redstone_wall_torch",                  76,  2   }, // unlit - facing south
	{ "minecraft:redstone_wall_torch",                  76,  3   }, // unlit - facing north
	{ "minecraft:redstone_wall_torch",                  76,  4   }, // unlit - facing up
	{ "minecraft:redstone_wall_torch",                  76,  5   }, // lit - facing east
	{ "minecraft:redstone_wall_torch",                  76,  6   }, // lit - facing west
	{ "minecraft:redstone_wall_torch",                  76,  7   }, // lit - facing south
	{ "minecraft:redstone_wall_torch",                  76,  8   }, // lit - facing north
	{ "minecraft:redstone_wall_torch",                  76,  9   }, // lit - facing up
	{ "minecraft:stone_button",                         77,  0   }, 
	{ "minecraft:polished_blackstone_button",           77,  1   }, // Added after flattening, data value not correct
	{ "minecraft:snow",                                 78,  0   }, // layer
	{ "minecraft:ice",                                  79,  0   }, 
	{ "minecraft:snow_block",                           80,  0   }, 
	{ "minecraft:cactus",                               81,  0   }, 
	{ "minecraft:clay",                                 82,  0   }, 
	{ "minecraft:sugar_cane",                           83,  0   }, 
	{ "minecraft:jukebox",                              84,  0   }, 
	{ "minecraft:oak_fence",                            85,  0   }, 
	{ "minecraft:crimson_fence",                        85,  1   }, 
	{ "minecraft:warped_fence",                         85,  2   }, 
	{ "minecraft:pumpkin",                              86,  0   }, 
	{ "minecraft:netherrack",                           87,  0   }, 
	{ "minecraft:soul_sand",                            88,  0   }, 
	{ "minecraft:glowstone",                            89,  0   }, 
	{ "minecraft:portal",                               90,  0   }, // portal long edge runs east-west
	{ "minecraft:portal",                               90,  1   }, // portal long edge runs north-south
	{ "minecraft:jack_o_lantern",                       91,  0   }, // facing east
	{ "minecraft:jack_o_lantern",                       91,  1   }, // facing north
	{ "minecraft:jack_o_lantern",                       91,  2   }, // facing south
	{ "minecraft:jack_o_lantern",                       91,  3   }, // facing west
	{ "minecraft:cake",                                 92,  0   },
	{ "minecraft:candle_cake",                          92,  1   },
	{ "minecraft:white_candle_cake",                    92,  2   },
	{ "minecraft:orange_candle_cake",                   92,  3   },
	{ "minecraft:magenta_candle_cake",                  92,  4   },
	{ "minecraft:light_blue_candle_cake",               92,  5   },
	{ "minecraft:yellow_candle_cake",                   92,  6   },
	{ "minecraft:lime_candle_cake",                     92,  7   },
	{ "minecraft:pink_candle_cake",                     92,  8   },
	{ "minecraft:gray_candle_cake",                     92,  9   },
	{ "minecraft:light_gray_candle_cake",               92,  10  },
	{ "minecraft:cyan_candle_cake",                     92,  11  },
	{ "minecraft:purple_candle_cake",                   92,  12  },
	{ "minecraft:blue_candle_cake",                     92,  13  },
	{ "minecraft:brown_candle_cake",                    92,  14  },
	{ "minecraft:green_candle_cake",                    92,  15  },
	{ "minecraft:red_candle_cake",                      92,  16  },
	{ "minecraft:black_candle_cake",                    92,  17  }, 
	{ "minecraft:repeater",                             93,  0   }, // unpowered repeater
	{ "minecraft:repeater",                             94,  0   }, // powered repeater
	{ "minecraft:white_stained_glass",                  95,  0   },
	{ "minecraft:orange_stained_glass",                 95,  1   },
	{ "minecraft:magenta_stained_glass",                95,  2   },
	{ "minecraft:light_blue_stained_glass",             95,  3   },
	{ "minecraft:yellow_stained_glass",                 95,  4   },
	{ "minecraft:lime_stained_glass",                   95,  5   },
	{ "minecraft:pink_stained_glass",                   95,  6   },
	{ "minecraft:gray_stained_glass",                   95,  7   },
	{ "minecraft:light_gray_stained_glass",             95,  8   },
	{ "minecraft:cyan_stained_glass",                   95,  9   },
	{ "minecraft:purple_stained_glass",                 95,  10  },
	{ "minecraft:blue_stained_glass",                   95,  11  },
	{ "minecraft:brown_stained_glass",                  95,  12  },
	{ "minecraft:green_stained_glass",                  95,  13  },
	{ "minecraft:red_stained_glass",                    95,  14  },
	{ "minecraft:black_stained_glass",                  95,  15  },
	{ "minecraft:oak_trapdoor",                         96,  0   },
	{ "minecraft:spruce_trapdoor",                      96,  1   },
	{ "minecraft:birch_trapdoor",                       96,  2   },
	{ "minecraft:jungle_trapdoor",                      96,  3   },
	{ "minecraft:acacia_trapdoor",                      96,  4   },
	{ "minecraft:dark_oak_trapdoor",                    96,  5   },
	{ "minecraft:crimson_trapdoor",                     96,  6   },
	{ "minecraft:warped_trapdoor",                      96,  7   },
	{ "minecraft:infested_stone",                       97,  0   },
	{ "minecraft:infested_cobblestone",                 97,  1   },
	{ "minecraft:infested_stone_bricks",                97,  2   },
	{ "minecraft:infested_cracked_stone_bricks",        97,  3   },
	{ "minecraft:infested_mossy_stone_bricks",          97,  4   },
	{ "minecraft:infested_chiseled_stone_bricks",       97,  5   },
	{ "minecraft:infested_deepslate",                   97,  6   },
	{ "minecraft:stone_bricks",                         98,  0   },
	{ "minecraft:cracked_stone_bricks",                 98,  1   },
	{ "minecraft:mossy_stone_bricks",                   98,  2   },
	{ "minecraft:chiseled_stone_bricks",                98,  3   },
	{ "minecraft:brown_mushroom_block",                 99,  0   },
	{ "minecraft:mushroom_stem",                        99,  1   },
	{ "minecraft:red_mushroom_block",                   100, 0   },
	{ "minecraft:iron_bars",                            101, 0   },
	{ "minecraft:glass_pane",                           102, 0   },
	{ "minecraft:melon",                                103, 0   },
	{ "minecraft:pumpkin_stem",                         104, 0   },
	{ "minecraft:attached_pumpkin_stem",                104, 1   },
	{ "minecraft:melon_stem",                           105, 0   }, 
	{ "minecraft:attached_melon_stem",                  105, 1   }, 
	{ "minecraft:vine",                                 106, 0   }, 
	{ "minecraft:oak_fence_gate",                       107, 0   }, 
	{ "minecraft:crimson_fence_gate",                   107, 1   }, 
	{ "minecraft:warped_fence_gate",                    107, 2   }, 
	{ "minecraft:brick_stairs", 108, 0  }, 
	{ "minecraft:stone_brick_stairs", 109, 0  }, 
	{ "minecraft:mycelium", 110, 0  }, 
	{ "minecraft:light_gray_concrete", 111, 0  }, 
	{ "minecraft:nether_brick", 112, 0  }, 
	{ "minecraft:nether_brick_fence",                   113, 0   }, 
	{ "minecraft:nether_brick_stairs", 114, 0  }, 
	{ "minecraft:nether_wart", 115, 0  }, 
	{ "minecraft:enchanting_table", 116, 0  }, 
	{ "minecraft:brewing_stand", 117, 0  }, 
	{ "minecraft:cauldron", 118, 0  }, 
	{ "minecraft:end_portal", 119, 0  }, 
	{ "minecraft:end_portal_frame", 120, 0  }, 
	{ "minecraft:end_stone", 121, 0  }, 
	{ "minecraft:dragon_egg", 122, 0  }, 
	{ "minecraft:redstone_lamp", 123, 0  }, 
	{ "minecraft:shroomlight", 124, 0  }, 
	{ "minecraft:oak_wood", 125, 0  }, 
	{ "minecraft:oak_slab", 126, 0  }, 
	{ "minecraft:cocoa", 127, 0  }, 
	{ "minecraft:sandstone_stairs", 128, 0  }, 
	{ "minecraft:emerald_ore", 129, 0  }, 
	{ "minecraft:ender_chest", 130, 0  }, 
	{ "minecraft:tripwire_hook", 131, 0  }, 
	{ "minecraft:tripwire", 132, 0  }, 
	{ "minecraft:emerald_block", 133, 0  }, 
    { "minecraft:spruce_stairs",                        134, 0   }, // facing east 
	{ "minecraft:spruce_stairs",                        134, 1   }, // facing north 
	{ "minecraft:spruce_stairs",                        134, 2   }, // facing south 
	{ "minecraft:spruce_stairs",                        134, 3   }, // facing west 
    { "minecraft:birch_stairs",                         135, 0   }, // facing east 
	{ "minecraft:birch_stairs",                         135, 1   }, // facing north 
	{ "minecraft:birch_stairs",                         135, 2   }, // facing south 
	{ "minecraft:birch_stairs",                         135, 3   }, // facing west 
    { "minecraft:jungle_stairs",                        136, 0   }, // facing east 
	{ "minecraft:jungle_stairs",                        136, 1   }, // facing north 
	{ "minecraft:jungle_stairs",                        136, 2   }, // facing south 
	{ "minecraft:jungle_stairs",                        136, 3   }, // facing west 
	{ "minecraft:command_block",                        137, 0   }, 
	{ "minecraft:beacon", 138, 0  }, 
	{ "minecraft:cobblestone_wall", 139, 0  }, 
	{ "minecraft:flower_pot", 140, 0  }, 
	{ "minecraft:carrots", 141, 0  }, 
	{ "minecraft:potatoes", 142, 0  }, 
	{ "minecraft:oak_button",                           143, 1   }, 
	{ "minecraft:spruce_button",                        143, 2   }, 
	{ "minecraft:birch_button",                         143, 3   }, 
	{ "minecraft:jungle_button",                        143, 4   }, 
	{ "minecraft:acacia_button",                        143, 5   }, 
	{ "minecraft:dark_oak_button",                      143, 6   }, 
	{ "minecraft:crimson_button",                       143, 7   }, // Added after flattening, data value not correct
	{ "minecraft:warped_button",                        143, 8   }, // Added after flattening, data value not correct
	{ "minecraft:skeleton_wall_skull", 144, 0  }, 
	{ "minecraft:anvil", 145, 0  }, 
	{ "minecraft:trapped_chest", 146, 0  }, 
	{ "minecraft:light_weighted_pressure_plate",        147, 0  }, 
	{ "minecraft:heavy_weighted_pressure_plate",        148, 0  }, 
	{ "minecraft:comparator", 149, 0  }, 
	{ "minecraft:chain", 150, 0  }, 
	{ "minecraft:daylight_detector", 151, 0  }, 
	{ "minecraft:redstone_block", 152, 0  }, 
	{ "minecraft:nether_quartz_ore", 153, 0  }, 
	{ "minecraft:hopper", 154, 0  }, 
	{ "minecraft:quartz_block", 155, 0  }, 
	{ "minecraft:quartz_stairs", 156, 0  }, 
	{ "minecraft:activator_rail", 157, 0  }, 
	{ "minecraft:dropper", 158, 0  }, 
	{ "minecraft:white_terracotta",                     159, 0   }, 
	{ "minecraft:orange_terracotta",                    159, 1   }, 
	{ "minecraft:magenta_terracotta",                   159, 2   }, 
	{ "minecraft:light_blue_terracotta",                159, 3   }, 
	{ "minecraft:yellow_terracotta",                    159, 4   }, 
	{ "minecraft:lime_terracotta",                      159, 5   }, 
	{ "minecraft:pink_terracotta",                      159, 6   }, 
	{ "minecraft:gray_terracotta",                      159, 7   }, 
	{ "minecraft:light_gray_terracotta",                159, 8   }, 
	{ "minecraft:cyan_terracotta",                      159, 9   }, 
	{ "minecraft:purple_terracotta",                    159, 10  }, 
	{ "minecraft:blue_terracotta",                      159, 11  }, 
	{ "minecraft:brown_terracotta",                     159, 12  }, 
	{ "minecraft:green_terracotta",                     159, 13  }, 
	{ "minecraft:red_terracotta",                       159, 14  }, 
	{ "minecraft:black_terracotta",                     159, 15  },	
    { "minecraft:white_stained_glass_pane",             160,  0  },
	{ "minecraft:orange_stained_glass_pane",            160,  1  },
	{ "minecraft:magenta_stained_glass_pane",           160,  2  },
	{ "minecraft:light_blue_stained_glass_pane",        160,  3  },
	{ "minecraft:yellow_stained_glass_pane",            160,  4  },
	{ "minecraft:lime_stained_glass_pane",              160,  5  },
	{ "minecraft:pink_stained_glass_pane",              160,  6  },
	{ "minecraft:gray_stained_glass_pane",              160,  7  },
	{ "minecraft:light_gray_stained_glass_pane",        160,  8  },
	{ "minecraft:cyan_stained_glass_pane",              160,  9  },
	{ "minecraft:purple_stained_glass_pane",            160,  10 },
	{ "minecraft:blue_stained_glass_pane",              160,  11 },
	{ "minecraft:brown_stained_glass_pane",             160,  12 },
	{ "minecraft:green_stained_glass_pane",             160,  13 },
	{ "minecraft:red_stained_glass_pane",               160,  14 },
	{ "minecraft:black_stained_glass_pane",             160,  15 },
	{ "minecraft:acacia_leaves",                        161, 0   }, // check for decay (if no log within 4 blocks decay)
	{ "minecraft:dark_oak_leaves",                      161, 1   }, // check for decay (if no log within 4 blocks decay)
	{ "minecraft:removed0_leaves",                      161, 2   }, // check for decay (if no log within 4 blocks decay)
	{ "minecraft:removed1_leaves",                      161, 3   }, // check for decay (if no log within 4 blocks decay)
	{ "minecraft:acacia_leaves",                        161, 4   }, // persistent
	{ "minecraft:dark_oak_leaves",                      161, 5   }, // persistent
	{ "minecraft:removed0_leaves",                      161, 6   }, // persistent
	{ "minecraft:removed1_leaves",                      161, 7   }, // persistent
	{ "minecraft:acacia_leaves",                        161, 8   }, // persistent (12-15 same as 8-11)
	{ "minecraft:dark_oak_leaves",                      161, 9   }, // persistent (12-15 same as 8-11)
	{ "minecraft:removed0_leaves",                      161, 10  }, // persistent (12-15 same as 8-11)
	{ "minecraft:removed1_leaves",                      161, 11  }, // persistent (12-15 same as 8-11)
	{ "minecraft:oak_wood", 162, 0  }, 
	{ "minecraft:spruce_wood", 162, 1  }, 
	{ "minecraft:birch_wood", 162, 2  }, 
	{ "minecraft:jungle_wood", 162, 3  }, 
	{ "minecraft:acacia_wood", 162, 4  }, 
	{ "minecraft:dark_oak_wood", 162, 5  }, 
	{ "minecraft:crimson_hyphae", 162, 6  }, 
	{ "minecraft:warped_hyphae", 162, 7  }, 
	{ "minecraft:stripped_oak_wood", 162, 8  }, 
	{ "minecraft:stripped_spruce_wood", 162, 9  }, 
	{ "minecraft:stripped_birch_wood", 162, 10  }, 
	{ "minecraft:stripped_jungle_wood", 162, 11 }, 
	{ "minecraft:stripped_acacia_wood", 162, 12 }, 
	{ "minecraft:stripped_crimson_hyphae", 162, 14 },
	{ "minecraft:stripped_warped_hyphae", 162, 15 }, 
    { "minecraft:acacia_stairs",                        163, 0   }, // facing east 
	{ "minecraft:acacia_stairs",                        163, 1   }, // facing north 
	{ "minecraft:acacia_stairs",                        163, 2   }, // facing south 
	{ "minecraft:acacia_stairs",                        163, 3   }, // facing west
    { "minecraft:dark_oak_stairs",                      164, 0   }, // facing east
	{ "minecraft:dark_oak_stairs",                      164, 1   }, // facing north
	{ "minecraft:dark_oak_stairs",                      164, 2   }, // facing south
	{ "minecraft:dark_oak_stairs",                      164, 3   }, // facing west
    { "minecraft:crimson_stairs",                       164, 4   }, // facing east	  - Added after flattening, data value not correct
	{ "minecraft:crimson_stairs",                       164, 5   }, // facing north	  - Added after flattening, data value not correct
	{ "minecraft:crimson_stairs",                       164, 6   }, // facing south	  - Added after flattening, data value not correct
	{ "minecraft:crimson_stairs",                       164, 7   }, // facing west	  - Added after flattening, data value not correct
    { "minecraft:warped_stairs",                        164, 8   }, // facing east	  - Added after flattening, data value not correct
	{ "minecraft:warped_stairs",                        164, 9   }, // facing north	  - Added after flattening, data value not correct
	{ "minecraft:warped_stairs",                        164, 10  }, // facing south	  - Added after flattening, data value not correct
	{ "minecraft:warped_stairs",                        164, 11  }, // facing west	  - Added after flattening, data value not correct
    { "minecraft:stone_stairs",                         164, 12  }, // facing east	  - Added after flattening, data value not correct
	{ "minecraft:stone_stairs",                         164, 13  }, // facing north	  - Added after flattening, data value not correct
	{ "minecraft:stone_stairs",                         164, 14  }, // facing south	  - Added after flattening, data value not correct
	{ "minecraft:stone_stairs",                         164, 15  }, // facing west	  - Added after flattening, data value not correct
	{ "minecraft:slime_block", 165, 0  }, 
	{ "minecraft:barrier", 166, 0  }, 
	{ "minecraft:iron_trapdoor",                        167, 0   }, 
	{ "minecraft:prismarine", 168, 0  }, 
	{ "minecraft:sea_lantern", 169, 0  }, 
	{ "minecraft:hay_block", 170, 0  }, 
	{ "minecraft:white_carpet", 171, 0  }, 
	{ "minecraft:terracotta", 172, 0  }, 
	{ "minecraft:coal_block", 173, 0  }, 
	{ "minecraft:packed_ice",                           174, 0   }, 
	{ "minecraft:sunflower",                            175, 0   }, // tall flowers, two-block lower - data values may not be correct
	{ "minecraft:lilac",                                175, 1   }, // tall flowers, two-block lower - data values may not be correct
	{ "minecraft:tall_grass",                           175, 2   }, // tall plants, two-block lower - data values may not be correct
	{ "minecraft:large_fern",                           175, 3   }, // tall plants, two-block lower - data values may not be correct
	{ "minecraft:rose_bush",                            175, 4   }, // tall flowers, two-block lower - data values may not be correct
	{ "minecraft:peony",                                175, 5   }, // tall flowers, two-block lower - data values may not be correct
	{ "minecraft:tall_seagrass",                        175, 6   }, // tall plants, two-block upper - Added after flattening, data value not correct
	{ "minecraft:white_banner", 176, 0  }, 
	{ "minecraft:white_wall_banner", 177, 0  }, 
	{ "minecraft:white_concrete_powder", 178, 0  }, 
	{ "minecraft:red_sandstone", 179, 0  }, 
    { "minecraft:red_sandstone", 179, 0  },
	{ "minecraft:chiseled_red_sandstone", 179, 1  },
	{ "minecraft:cut_red_sandstone", 179, 2  },
	{ "minecraft:smooth_red_sandstone", 179, 3  },
	{ "minecraft:red_sandstone_stairs", 180, 0  }, 
	{ "minecraft:red_sandstone_wall", 181, 0  }, 
	{ "minecraft:red_sandstone_slab", 182, 0  },
	{ "minecraft:spruce_fence_gate",                    183, 0   }, 
	{ "minecraft:birch_fence_gate",                     184, 0   }, 
	{ "minecraft:jungle_fence_gate",                    185, 0   }, 
	{ "minecraft:dark_oak_fence_gate",                  186, 0   }, 
	{ "minecraft:acacia_fence_gate",                    187, 0   }, 
	{ "minecraft:spruce_fence",                         188, 0   }, 
	{ "minecraft:birch_fence",                          189, 0   }, 
	{ "minecraft:jungle_fence",                         190, 0   }, 
	{ "minecraft:dark_oak_fence",                       191, 0   }, 
	{ "minecraft:acacia_fence",                         192, 0   },
    { "minecraft:spruce_door",                          193, 0   }, // facing east
    { "minecraft:spruce_door",                          193, 1   }, // facing north
    { "minecraft:spruce_door",                          193, 2   }, // facing south
    { "minecraft:spruce_door",                          193, 3   }, // facing west
	{ "minecraft:birch_door",                           194, 0   }, // facing east
	{ "minecraft:birch_door",                           194, 1   }, // facing north
	{ "minecraft:birch_door",                           194, 2   }, // facing south
	{ "minecraft:birch_door",                           194, 3   }, // facing west
	{ "minecraft:jungle_door",                          195, 0   }, // facing east
	{ "minecraft:jungle_door",                          195, 1   }, // facing north
	{ "minecraft:jungle_door",                          195, 2   }, // facing south
	{ "minecraft:jungle_door",                          195, 3   }, // facing west
	{ "minecraft:acacia_door",                          196, 0   }, // facing east
	{ "minecraft:acacia_door",                          196, 1   }, // facing north
	{ "minecraft:acacia_door",                          196, 2   }, // facing south
	{ "minecraft:acacia_door",                          196, 3   }, // facing west
    { "minecraft:dark_oak_door",                        197, 0   }, // facing east
	{ "minecraft:dark_oak_door",                        197, 1   }, // facing north
	{ "minecraft:dark_oak_door",                        197, 2   }, // facing south
	{ "minecraft:dark_oak_door",                        197, 3   }, // facing west
    { "minecraft:crimson_door",                         197, 4   }, // facing east	 - Added after flattening, data value not correct
	{ "minecraft:crimson_door",                         197, 5   }, // facing north	 - Added after flattening, data value not correct
	{ "minecraft:crimson_door",                         197, 6   }, // facing south	 - Added after flattening, data value not correct
	{ "minecraft:crimson_door",                         197, 7   }, // facing west	 - Added after flattening, data value not correct
    { "minecraft:warped_door",                          197, 8   }, // facing east	 - Added after flattening, data value not correct
	{ "minecraft:warped_door",                          197, 9   }, // facing north	 - Added after flattening, data value not correct
	{ "minecraft:warped_door",                          197, 10  }, // facing south - Added after flattening, data value not correct
	{ "minecraft:warped_door",                          197, 11  }, // facing west	 - Added after flattening, data value not correct
	{ "minecraft:end_rod", 198, 0  }, 
	{ "minecraft:chorus_plant", 199, 0  }, 
	{ "minecraft:chorus_flower", 200, 0  }, 
	{ "minecraft:purpur_block", 201, 0  }, 
	{ "minecraft:purpur_pillar", 202, 0  }, 
	{ "minecraft:purpur_stairs", 203, 0  }, 
	{ "minecraft:purple_stained_glass", 204, 0  }, 
	{ "minecraft:purpur_slab", 205, 0  }, 
	{ "minecraft:end_stone_bricks", 206, 0  }, 
	{ "minecraft:beetroots", 207, 0  }, 
	{ "minecraft:grass_path", 208, 0  }, 
	{ "minecraft:end_gateway", 209, 0  }, 
	{ "minecraft:repeating_command_block", 210, 0  }, 
	{ "minecraft:chain_command_block", 211, 0  }, 
	{ "minecraft:frosted_ice",                          212, 0   }, 
	{ "minecraft:magma_block", 213, 0  }, 
	{ "minecraft:nether_wart_block", 214, 0  }, 
	{ "minecraft:red_nether_bricks", 215, 0  }, 
	{ "minecraft:bone_block", 216, 0  }, 
	{ "minecraft:structure_void", 217, 0  }, 
	{ "minecraft:observer", 218, 0  }, 
	{ "minecraft:white_shulker_box", 219, 0  }, 
	{ "minecraft:orange_shulker_box", 220, 0  }, 
	{ "minecraft:magenta_shulker_box", 221, 0  }, 
	{ "minecraft:light_blue_shulker_box", 222, 0  }, 
	{ "minecraft:yellow_shulker_box", 223, 0  }, 
	{ "minecraft:lime_shulker_box", 224, 0  }, 
	{ "minecraft:pink_shulker_box", 225, 0  }, 
	{ "minecraft:gray_shulker_box", 226, 0  }, 
	{ "minecraft:light_gray_shulker_box", 227, 0  }, 
	{ "minecraft:cyan_shulker_box", 228, 0  }, 
	{ "minecraft:purple_shulker_box", 229, 0  }, 
	{ "minecraft:blue_shulker_box", 230, 0  }, 
	{ "minecraft:brown_shulker_box", 231, 0  }, 
	{ "minecraft:green_shulker_box", 232, 0  }, 
	{ "minecraft:red_shulker_box", 233, 0  }, 
	{ "minecraft:black_shulker_box", 234, 0  },
	{ "minecraft:white_glazed_terracotta", 235, 0  }, 
	{ "minecraft:orange_glazed_terracotta", 236, 0  }, 
	{ "minecraft:magenta_glazed_terracotta", 237, 0  }, 
	{ "minecraft:light_blue_glazed_terracotta", 238, 0  }, 
	{ "minecraft:yellow_glazed_terracotta", 239, 0  }, 
	{ "minecraft:lime_glazed_terracotta", 240, 0  }, 
	{ "minecraft:pink_glazed_terracotta", 241, 0  }, 
	{ "minecraft:gray_glazed_terracotta", 242, 0  }, 
	{ "minecraft:light_gray_glazed_terracotta", 243, 0  }, 
	{ "minecraft:cyan_glazed_terracotta", 244, 0  }, 
	{ "minecraft:purple_glazed_terracotta", 245, 0  }, 
	{ "minecraft:blue_glazed_terracotta", 246, 0  }, 
	{ "minecraft:brown_glazed_terracotta", 247, 0  }, 
	{ "minecraft:green_glazed_terracotta", 248, 0  }, 
	{ "minecraft:red_glazed_terracotta", 249, 0  }, 
	{ "minecraft:black_glazed_terracotta", 250, 0  }, 
	{ "minecraft:white_concrete", 251,  0   }, 
	{ "minecraft:orange_concrete", 251,  1   }, 
	{ "minecraft:magenta_concrete", 251,  2   }, 
	{ "minecraft:light_blue_concrete", 251,  3   }, 
	{ "minecraft:yellow_concrete", 251,  4   }, 
	{ "minecraft:lime_concrete", 251,  5   }, 
	{ "minecraft:pink_concrete", 251,  6   }, 
	{ "minecraft:gray_concrete", 251, 7  }, 
	{ "minecraft:light_gray_concrete", 251, 8  }, 
	{ "minecraft:cyan_concrete", 251, 9  }, 
	{ "minecraft:purple_concrete", 251, 10 }, 
	{ "minecraft:blue_concrete", 251, 11 }, 
	{ "minecraft:brown_concrete", 251, 12 }, 
	{ "minecraft:green_concrete", 251, 13 }, 
	{ "minecraft:red_concrete", 251, 14 }, 
	{ "minecraft:black_concrete", 251, 15 },
	{ "minecraft:white_concrete_powder", 252,  0   }, 
	{ "minecraft:orange_concrete_powder", 252,  1   }, 
	{ "minecraft:magenta_concrete_powder", 252,  2   }, 
	{ "minecraft:light_blue_concrete_powder", 252,  3   }, 
	{ "minecraft:yellow_concrete_powder", 252,  4   }, 
	{ "minecraft:lime_concrete_powder", 252,  5   }, 
	{ "minecraft:pink_concrete_powder", 252,  6   }, 
	{ "minecraft:gray_concrete_powder", 252, 7  }, 
	{ "minecraft:light_gray_concrete_powder", 252, 8  }, 
	{ "minecraft:cyan_concrete_powder", 252, 9  }, 
	{ "minecraft:purple_concrete_powder", 252, 10 }, 
	{ "minecraft:blue_concrete_powder", 252, 11 }, 
	{ "minecraft:brown_concrete_powder", 252, 12 }, 
	{ "minecraft:green_concrete_powder", 252, 13 }, 
	{ "minecraft:red_concrete_powder", 252, 14 }, 
	{ "minecraft:black_concrete_powder", 252, 15 }, 
	{ "minecraft:unused_253", 253, 0  }, // pre-flattening unused blockid, added here to ensure have all old id's covered
	{ "minecraft:unused_254", 254, 0  }, // pre-flattening unused blockid, added here to ensure have all old id's covered
	{ "minecraft:structure_block", 255, 0 }
};

static const uint32_t numDefaultNamespaceAndBlockIDs = (uint32_t)sizeof(defaultNamespaceAndBlockIDs)/sizeof(enkiMINamespaceAndBlockID);

typedef struct SectionChunkInfo_s
{
	uint8_t offset_0;
	uint8_t offset_1;
	uint8_t offset_2;
	uint8_t sectorCount;
} SectionChunkInfo;

static int32_t GetChunkLocation( SectionChunkInfo section_ )
{
	return ( ( ( section_.offset_0 << 16 ) + ( section_.offset_1 << 8 ) + section_.offset_2 ) * SECTOR_SIZE );
}

typedef struct BigEndian4BytesTo32BitInt_s
{
	uint8_t pos[4];
} BigEndian4BytesTo32BitInt;


static int32_t Get32BitInt( BigEndian4BytesTo32BitInt in_ )
{
	return ( ( in_.pos[0] << 24 ) + ( in_.pos[1] << 16 ) + ( in_.pos[2] << 8 ) + in_.pos[3] );
}

typedef struct RegionHeader_s
{
	SectionChunkInfo          sectionChunksInfos[ ENKI_MI_REGION_CHUNKS_NUMBER ]; // chunks locations
	BigEndian4BytesTo32BitInt sectionChunksTimestamps[ ENKI_MI_REGION_CHUNKS_NUMBER ]; // chunks timestamps
} RegionHeader;

const char * enkiGetNBTTagIDAsString( uint8_t tagID_ )
{
	return tagIdString[ tagID_ ];
}

const char * enkiGetNBTTagHeaderIDAsString( enkiNBTTagHeader tagheader_ )
{
	return tagIdString[ tagheader_.tagId ];
}

void enkiNBTInitFromMemoryUncompressed( enkiNBTDataStream* pStream_, uint8_t * pData_, uint32_t dataSize_)
{
	memset( &pStream_->currentTag, 0, sizeof( enkiNBTTagHeader ) );
	pStream_->pData = pData_;
	pStream_->dataLength = dataSize_;
	pStream_->pCurrPos = pStream_->pData;
	pStream_->pDataEnd = pStream_->pData + pStream_->dataLength;
	pStream_->pNextTag = pStream_->pCurrPos;
	pStream_->level = -1;
	pStream_->pAllocations = NULL;
}

int enkiNBTInitFromMemoryCompressed( enkiNBTDataStream* pStream_, uint8_t * pCompressedData_,
										uint32_t compressedDataSize_, uint32_t uncompressedSizeHint_)
{
	// check if gzip style first:  https://tools.ietf.org/html/rfc1952#section-2.2
	static const uint32_t GZIP_HEADER_SIZE = 10;
	if(    compressedDataSize_ > GZIP_HEADER_SIZE
		&& pCompressedData_[0] == 0x1f
		&& pCompressedData_[1] == 0x8b )
	{
		// gzip style
		if( pCompressedData_[3] != 0)
		{
			// flags add extra information, normally not added by minecraft and we don't handle them
			enkiNBTInitFromMemoryUncompressed( pStream_, NULL, 0 );
			return 0;
		}

		int32_t ISIZE = *(int32_t*)(pCompressedData_ + compressedDataSize_ - 4);
		assert(ISIZE > 0);
		uint8_t* gzUncompressedData = (uint8_t*)malloc( (size_t)ISIZE );

		// uncompress gzip
		mz_stream stream;
		int status;
		memset(&stream, 0, sizeof(stream));
		uint32_t gzCompressedDataSize = compressedDataSize_ - GZIP_HEADER_SIZE;
		uint8_t* gzCompressedData     = pCompressedData_    + GZIP_HEADER_SIZE;

		stream.next_in   = gzCompressedData;
		stream.avail_in  = (mz_uint32)gzCompressedDataSize;
		stream.next_out  = gzUncompressedData;
		stream.avail_out = (mz_uint32)ISIZE;

		status = mz_inflateInit2(&stream, -MZ_DEFAULT_WINDOW_BITS);
		if (status == MZ_OK)
		{
			status = mz_inflate(&stream, MZ_FINISH);
			if (status != MZ_STREAM_END)
			{
				mz_inflateEnd(&stream);
				status = ((status == MZ_BUF_ERROR) && (!stream.avail_in)) ? MZ_DATA_ERROR : status;
			}
			else
			{
				status = mz_inflateEnd(&stream);
			}

			if( status == MZ_OK )
			{
				enkiNBTInitFromMemoryUncompressed( pStream_, gzUncompressedData, ( uint32_t )stream.total_out );
				enkiNBTAddAllocation( pStream_, gzUncompressedData );
				return 1;
			}
			else
			{
				free( gzUncompressedData );
			}
		}
	}


	mz_ulong destLength = uncompressedSizeHint_;
	if( destLength <= compressedDataSize_ )
	{
		destLength = compressedDataSize_ * 4 + 1024; // estimate uncompressed size
	}
	mz_ulong startDestLength = destLength;
	uint8_t* dataUnCompressed = (uint8_t*)malloc( destLength );
	int retval = uncompress( dataUnCompressed, &destLength, pCompressedData_, compressedDataSize_ );
	if( retval == MZ_BUF_ERROR && startDestLength == destLength )
	{
		// failed to uncompress, buffer full
		for( int attempts = 0; ( retval != MZ_OK ) && ( attempts < 3 ); ++attempts )
		{
			free( dataUnCompressed );
			destLength *= 4 + 1024;
			dataUnCompressed = (uint8_t*)malloc( destLength );
			retval = uncompress( dataUnCompressed, &destLength, pCompressedData_, compressedDataSize_ );
		}
	}
	if( retval != MZ_OK )
	{
		enkiNBTInitFromMemoryUncompressed( pStream_, NULL, 0 );
		free( dataUnCompressed );
		return 0;
	}

	dataUnCompressed = (uint8_t*)realloc( dataUnCompressed, destLength ); // reallocate to actual size
	enkiNBTInitFromMemoryUncompressed( pStream_, dataUnCompressed, ( uint32_t )destLength );
	enkiNBTAddAllocation( pStream_, dataUnCompressed );
	return 1;
}

void enkiNBTAddAllocation( enkiNBTDataStream* pStream_, void* pAllocation_ )
{
	enkiNBTAllocation* pAlloc = (enkiNBTAllocation*)malloc(sizeof(enkiNBTAllocation));
	pAlloc->pAllocation = pAllocation_;
	pAlloc->pNext = pStream_->pAllocations;
	pStream_->pAllocations = pAlloc;
}

void enkiNBTFreeAllocations( enkiNBTDataStream* pStream_ )
{
	enkiNBTAllocation* pNext = pStream_->pAllocations;
	while( pNext )
	{
		enkiNBTAllocation* pCurr = pNext;
		free( pCurr->pAllocation );
		pNext = pCurr->pNext;
		free( pCurr );
	}
	memset( pStream_, 0, sizeof(enkiNBTDataStream) );
}

int8_t  enkiNBTReadInt8( enkiNBTDataStream* pStream_ )
{
	int8_t retVal = pStream_->pCurrPos[ 0 ];
	pStream_->pCurrPos += 1;
	return retVal;
}

int8_t  enkiNBTReadByte( enkiNBTDataStream* pStream_ )
{
	return enkiNBTReadInt8( pStream_ );
}

int16_t enkiNBTReadInt16( enkiNBTDataStream* pStream_ )
{
	int16_t retVal = ( pStream_->pCurrPos[ 0 ] << 8 ) + pStream_->pCurrPos[ 1 ];
	pStream_->pCurrPos += 2;
	return retVal;
}

int16_t enkiNBTReadShort( enkiNBTDataStream* pStream_ )
{
	return enkiNBTReadInt16( pStream_ );
}

int32_t enkiNBTReadInt32( enkiNBTDataStream* pStream_ )
{
	int32_t retVal = ( pStream_->pCurrPos[ 0 ] << 24 ) + ( pStream_->pCurrPos[ 1 ] << 16 ) + ( pStream_->pCurrPos[ 2 ] << 8 ) + pStream_->pCurrPos[ 3 ];
	pStream_->pCurrPos += 4;
	return retVal;
}

int32_t enkiNBTReadInt( enkiNBTDataStream* pStream_ )
{
	return enkiNBTReadInt32( pStream_ );
}

float   enkiNBTReadFloat( enkiNBTDataStream* pStream_ )
{
	int32_t iVal = ( pStream_->pCurrPos[ 0 ] << 24 ) + ( pStream_->pCurrPos[ 1 ] << 16 ) + ( pStream_->pCurrPos[ 2 ] << 8 ) + pStream_->pCurrPos[ 3 ];
	float retVal = *( float* )&iVal;
	pStream_->pCurrPos += 4;
	return retVal;
}

int64_t enkiNBTReadInt64( enkiNBTDataStream* pStream_ )
{
	int64_t retVal = ( ( int64_t )pStream_->pCurrPos[ 0 ] << 54 ) + ( ( int64_t )pStream_->pCurrPos[ 1 ] << 48 ) + ( ( int64_t )pStream_->pCurrPos[ 2 ] << 40 ) + ( ( int64_t )pStream_->pCurrPos[ 5 ] << 32 ) + 
		             ( pStream_->pCurrPos[ 4 ] << 24 ) + ( pStream_->pCurrPos[ 5 ] << 16 ) + ( pStream_->pCurrPos[ 6 ] << 8 ) + pStream_->pCurrPos[ 7 ];
	pStream_->pCurrPos += 8;
	return retVal;
}

int64_t enkiNBTReadlong( enkiNBTDataStream* pStream_ )
{
	return enkiNBTReadInt64( pStream_ );
}

double  enkiNBTReadDouble( enkiNBTDataStream* pStream_ )
{
	int64_t iVal = ( ( int64_t )pStream_->pCurrPos[ 0 ] << 54 ) + ( ( int64_t )pStream_->pCurrPos[ 1 ] << 48 ) + ( ( int64_t )pStream_->pCurrPos[ 2 ] << 40 ) + ( ( int64_t )pStream_->pCurrPos[ 5 ] << 32 ) + 
		           ( pStream_->pCurrPos[ 4 ] << 24 ) + ( pStream_->pCurrPos[ 5 ] << 16 ) + ( pStream_->pCurrPos[ 6 ] << 8 ) + pStream_->pCurrPos[ 7 ];
	double retVal = *( double* )&iVal;
	pStream_->pCurrPos += 8;
	return retVal;
}

// Internal only uint16_t type
uint16_t enkiNBTReadUint16( enkiNBTDataStream* pStream_ )
{
	uint16_t retVal = ( pStream_->pCurrPos[ 0 ] << 8 ) + pStream_->pCurrPos[ 1 ];
	pStream_->pCurrPos += 2;
	return retVal;
}

enkiNBTString enkiNBTReadString( enkiNBTDataStream* pStream_ )
{
	enkiNBTString nbtString;
	nbtString.size = enkiNBTReadUint16( pStream_ );
	nbtString.pStrNotNullTerminated = (const char*)pStream_->pCurrPos;
	return nbtString;
}

static void SkipDataToNextTag( enkiNBTDataStream* pStream_ )
{
	uint8_t* pCurrPos = pStream_->pCurrPos;
	switch( pStream_->currentTag.tagId )
	{
	case enkiNBTTAG_End:
		// no data, so do nothing.
		break;
	case enkiNBTTAG_Byte:
		pStream_->pNextTag += 1; // 1 byte
		break;
	case enkiNBTTAG_Short:
		pStream_->pNextTag += 2; // 2 bytes
		break;
	case enkiNBTTAG_Int:
		pStream_->pNextTag += 4;
		break;
	case enkiNBTTAG_Long:
		pStream_->pNextTag += 8;
		break;
	case enkiNBTTAG_Float:
		pStream_->pNextTag += 4;
		break;
	case enkiNBTTAG_Double:
		pStream_->pNextTag += 8;
		break;
	case enkiNBTTAG_Byte_Array:
	{
		int32_t length = enkiNBTReadInt32( pStream_ );
		pStream_->pNextTag = pStream_->pCurrPos + length * 1; // array of bytes
		break;
	}
	case enkiNBTTAG_String:
	{
		int32_t length = enkiNBTReadUint16( pStream_ );
		pStream_->pNextTag = pStream_->pCurrPos + length;
		break;
	}
	case enkiNBTTAG_List:
		// read as a compound type
		break;
	case enkiNBTTAG_Compound:
		// data is in standard format, so do nothing.
		break;
	case enkiNBTTAG_Int_Array:
	{
		int32_t length = enkiNBTReadInt32( pStream_ );
		pStream_->pNextTag = pStream_->pCurrPos + length * 4; // array of ints (4 bytes)
		break;
	}
	case enkiNBTTAG_Long_Array:
	{
		int32_t length = enkiNBTReadInt32( pStream_ );
		pStream_->pNextTag = pStream_->pCurrPos + length * 8; // array of longs (4 bytes)
		break;
	}
	default:
		assert( 0 );
		break;
	}
	pStream_->pCurrPos = pCurrPos; // restore current position
}

int enkiNBTReadNextTag( enkiNBTDataStream* pStream_ )
{
	if( ( enkiNBTTAG_Compound == pStream_->currentTag.tagId ) || ( enkiNBTTAG_List == pStream_->currentTag.tagId ) )
	{
		pStream_->level++;
		if( pStream_->level == 512 )
		{
			assert(0); // in debug break.
			return 0; // invalid nested tags
		}
		pStream_->parentTags[ pStream_->level ] = pStream_->currentTag;
	}
	while( ( pStream_->level >= 0 ) && ( enkiNBTTAG_List == pStream_->parentTags[ pStream_->level ].tagId ) )
	{
		if( pStream_->parentTags[ pStream_->level ].listCurrItem == pStream_->parentTags[ pStream_->level ].listNumItems )
		{
			pStream_->level--;
		}
		else
		{
			pStream_->currentTag.tagId = pStream_->parentTags[ pStream_->level ].listItemTagId;
			pStream_->currentTag.pName = NULL;
			if( enkiNBTTAG_List == pStream_->currentTag.tagId )
			{
				pStream_->currentTag.listItemTagId = *(pStream_->pCurrPos++);
				pStream_->currentTag.listNumItems = enkiNBTReadInt32( pStream_ );
				pStream_->currentTag.listCurrItem = 0;
				pStream_->pNextTag = pStream_->pCurrPos;
			}
			SkipDataToNextTag( pStream_ );
			pStream_->pCurrPos = pStream_->pNextTag;
			pStream_->parentTags[ pStream_->level ].listCurrItem++;
			return 1;
		}
	}
	if( pStream_->pNextTag >= pStream_->pDataEnd )
	{
		return 0;
	}
	pStream_->pCurrPos = pStream_->pNextTag;

	// Get Tag Header
	pStream_->currentTag.pName = NULL;
	assert( *(pStream_->pCurrPos) < enkiNBTTAG_SIZE );

	pStream_->currentTag.tagId = *(pStream_->pCurrPos++);
	if( enkiNBTTAG_End != pStream_->currentTag.tagId )
	{
		// We null terminate tag names by replacing 2 byte size with 1 byte 0xFF, moving and adding 0 at end
		// This assumes no tag name is ever > in16_t max as name sizes are actually uint16_t
		if( 0xff == *(pStream_->pCurrPos) )
		{
			pStream_->pCurrPos++;
			pStream_->currentTag.pName = ( char* )pStream_->pCurrPos;
			while( *(pStream_->pCurrPos++) != 0 );
		}
		else
		{
			int32_t lengthOfName = enkiNBTReadInt16( pStream_ );
			if( lengthOfName )
			{
				assert( pStream_->pCurrPos+lengthOfName < pStream_->pDataEnd );
				// move and null terminate, flag as 
				*( pStream_->pCurrPos - 2 ) = 0xff; // this value will not be seen as a length since it will be negative
				pStream_->currentTag.pName = ( char* )( pStream_->pCurrPos - 1 );
				memmove( pStream_->currentTag.pName, pStream_->pCurrPos, lengthOfName );
				pStream_->pCurrPos += lengthOfName - 1;
				pStream_->pCurrPos[ 0 ] = 0; // null terminator
				pStream_->pCurrPos += 1;
			}
		}
	}
	if( enkiNBTTAG_List == pStream_->currentTag.tagId )
	{
		pStream_->currentTag.listItemTagId = *(pStream_->pCurrPos++);
		pStream_->currentTag.listNumItems = enkiNBTReadInt32( pStream_ );
		pStream_->currentTag.listCurrItem = 0;
	}
	pStream_->pNextTag = pStream_->pCurrPos;

	SkipDataToNextTag( pStream_ );

	if( ( pStream_->level >= 0 ) && ( enkiNBTTAG_End == pStream_->currentTag.tagId ) )
	{
		pStream_->level--;
	}

	return 1;
}


void enkiNBTRewind( enkiNBTDataStream* pStream_ )
{
	memset( &(pStream_->currentTag), 0, sizeof( enkiNBTTagHeader ) );
	pStream_->pCurrPos = pStream_->pData;
	pStream_->level = -1;
	pStream_->pNextTag = pStream_->pData;
}

void enkiRegionFileInit( enkiRegionFile* pRegionFile_ )
{
	memset( pRegionFile_, 0, sizeof( enkiRegionFile ) );
}


enkiRegionFile enkiRegionFileLoad( FILE * fp_ )
{
	enkiRegionFile regionFile;
	enkiRegionFileInit( &regionFile );
	fseek( fp_, 0, SEEK_END );
	regionFile.regionDataSize = ftell( fp_ );
	fseek( fp_, 0, SEEK_SET ); // return to start position

	if( regionFile.regionDataSize )
	{
		// get the data in the chunks data section
		regionFile.pRegionData = (uint8_t*)malloc( regionFile.regionDataSize );
		fread( regionFile.pRegionData, 1, regionFile.regionDataSize, fp_ ); // note: because sectionDataChunks is an array of single bytes, sizeof( sectionDataChunks ) == sectionDataSize
	}

	return regionFile;
}


uint8_t enkiHasChunk( enkiRegionFile regionFile_, int32_t chunkNr_ )
{
	uint8_t hasChunk = 0;
	RegionHeader* header = (RegionHeader*)regionFile_.pRegionData;
	uint32_t locationOffset = GetChunkLocation( header->sectionChunksInfos[ chunkNr_ ] );
	if( locationOffset >= sizeof( RegionHeader ) && ( locationOffset + 6 ) <= regionFile_.regionDataSize )
	{
		uint32_t length = Get32BitInt(  *( BigEndian4BytesTo32BitInt* )&regionFile_.pRegionData[ locationOffset ] );
		if( ( length + locationOffset + 4 ) <= regionFile_.regionDataSize )
		{
			hasChunk = 1;
		}
	}
	return hasChunk;
}

void enkiInitNBTDataStreamForChunk( enkiRegionFile regionFile_, int32_t chunkNr_, enkiNBTDataStream* pStream_ )
{
	enkiNBTInitFromMemoryUncompressed( pStream_, NULL, 0 ); // clears stream

	RegionHeader* header = (RegionHeader*)regionFile_.pRegionData;
	uint32_t locationOffset = GetChunkLocation( header->sectionChunksInfos[ chunkNr_ ] );
	if( locationOffset >= sizeof( RegionHeader ) && ( locationOffset + 6 ) <= regionFile_.regionDataSize )
	{
		uint32_t length = Get32BitInt(  *( BigEndian4BytesTo32BitInt* )&regionFile_.pRegionData[ locationOffset ] );
		uint8_t compression_type = regionFile_.pRegionData[ locationOffset + 4 ]; // we ignore this as unused for now
		assert(compression_type == 2 );
		(void)compression_type;
		--length; // length includes compression_type
		// get the data and decompress it
		if( ( length + locationOffset + 5 ) <= regionFile_.regionDataSize )
		{
			uint8_t* dataCompressed = &regionFile_.pRegionData[ locationOffset + 5 ];
			enkiNBTInitFromMemoryCompressed( pStream_, dataCompressed, length, 0 );
		}
	}
}

int32_t enkiGetTimestampForChunk( enkiRegionFile regionFile_, int32_t chunkNr_ )
{
	RegionHeader* header = (RegionHeader*)regionFile_.pRegionData;
	return Get32BitInt( header->sectionChunksTimestamps[ chunkNr_ ] );
}

void enkiRegionFileFreeAllocations(enkiRegionFile * pRegionFile_)
{
	free( pRegionFile_->pRegionData );
	memset( pRegionFile_, 0, sizeof(enkiRegionFile) );
}

int enkiAreStringsEqual( const char * lhs_, const char * rhs_ )
{
	if( lhs_ && rhs_ )
	{
		if( 0 == strcmp( lhs_, rhs_ ) )
		{
			return 1;
		}
	}
	return 0;
}

void enkiChunkInit( enkiChunkBlockData* pChunk_ )
{
	memset( pChunk_, 0, sizeof( enkiChunkBlockData ) );
}

enkiChunkBlockData enkiNBTReadChunk( enkiNBTDataStream * pStream_ )
{
	enkiChunkBlockData chunk;
	enkiChunkInit( &chunk );
	int foundSectionData = 0;
	while( enkiNBTReadNextTag( pStream_ ) )
	{
		if( enkiAreStringsEqual( "DataVersion", pStream_->currentTag.pName ) )
		{
			chunk.dataVersion = enkiNBTReadInt(pStream_ );
		}

		if( enkiAreStringsEqual( "Level", pStream_->currentTag.pName ) )
		{
			int foundXPos = 0;
			int foundZPos = 0;
			int foundSection = 0;
			int level = pStream_->level;
			while( enkiNBTReadNextTag( pStream_ ) && pStream_->level > level )
			{
				if( enkiAreStringsEqual( "xPos", pStream_->currentTag.pName ) )
				{
					foundXPos = 1;
					chunk.xPos = enkiNBTReadInt32( pStream_ );
				}
				else if( enkiAreStringsEqual( "zPos", pStream_->currentTag.pName ) )
				{
					foundZPos = 1;
					chunk.zPos = enkiNBTReadInt32( pStream_ );
				}
				else if( enkiAreStringsEqual( "Sections", pStream_->currentTag.pName ) )
				{
					foundSection = 1;
					int32_t levelParent = pStream_->level;
					int8_t sectionY = 0;
					uint8_t* pBlocks = NULL;
					uint8_t* pData = NULL;
					uint8_t* pBlockStates = NULL;
					enkiChunkSectionPalette sectionPalette = {0};
					do
					{
						if( 0 == enkiNBTReadNextTag( pStream_ ) )
						{
							break;
						}

						if( enkiAreStringsEqual( "Blocks", pStream_->currentTag.pName ) )
						{
							enkiNBTReadInt32( pStream_ ); // read number of items to advance pCurrPos to start of array
							pBlocks = pStream_->pCurrPos;
						}
						// TODO: process Data and Add sections
						 // https://minecraft.fandom.com/el/wiki/Chunk_format
						// Add: May not exist. 2048 bytes of additional block ID data. The value to add to (combine with) the above block ID to form the true block ID in the range 0 to 4095. 4 bits per block. Combining is done by shifting this value to the left 8 bits and then adding it to the block ID from above.
                        // Data: 2048 bytes of block data additionally defining parts of the terrain. 4 bits per block.
						if( enkiAreStringsEqual( "Add", pStream_->currentTag.pName ) )
						{
							enkiNBTReadInt32( pStream_ ); // read number of items to advance pCurrPos to start of array
							// NOT YET HANDLED
						}
						if( enkiAreStringsEqual( "Data", pStream_->currentTag.pName ) )
						{
							enkiNBTReadInt32( pStream_ ); // read number of items to advance pCurrPos to start of array
							pData = pStream_->pCurrPos;
						}
						if( enkiAreStringsEqual( "Y", pStream_->currentTag.pName ) )
						{
							// sectionY is not always present, and may indicate a start point.
							// For example, can find sectionY = -1 as first section, then next
							// section has data but no sectionY.
							sectionY = enkiNBTReadInt8( pStream_ );
						}
						if( enkiAreStringsEqual( "BlockStates", pStream_->currentTag.pName ) )
						{
							sectionPalette.blockArraySize = enkiNBTReadInt32( pStream_ ); // read number of items to advance pCurrPos to start of array
							pBlockStates = pStream_->pCurrPos;
						}
						if( enkiAreStringsEqual( "Palette", pStream_->currentTag.pName ) )
						{
							sectionPalette.size = (uint32_t)pStream_->currentTag.listNumItems;
							uint32_t numBits = 4;
							uint32_t test = (sectionPalette.size-1) >> 4;
							while( test )
							{
								test = test >> 1;
								++numBits;
							}
							sectionPalette.numBitsPerBlock = numBits;

							sectionPalette.pDefaultBlockIndex = (int32_t*)malloc(sizeof(int32_t)*sectionPalette.size);
							enkiNBTAddAllocation( pStream_, sectionPalette.pDefaultBlockIndex );
							sectionPalette.pNamespaceIDStrings = (enkiNBTString*)malloc(sizeof(enkiNBTString)*sectionPalette.size);
							enkiNBTAddAllocation( pStream_, sectionPalette.pNamespaceIDStrings );
							// read palettes
							int levelPalette = pStream_->level;
   						    uint32_t paletteNum = 0;
							while(     enkiNBTReadNextTag( pStream_ )
									&& levelPalette < pStream_->level )
							{
								if(   pStream_->currentTag.tagId == enkiNBTTAG_String
									&& enkiAreStringsEqual( "Name", pStream_->currentTag.pName ) )
								{
									assert( paletteNum < sectionPalette.size );
									enkiNBTString paletteEntry = enkiNBTReadString( pStream_ );
									// find in palette
									// enkiMIBlockID defaultBlockIDs[]
									sectionPalette.pDefaultBlockIndex[ paletteNum ] = -1;
									sectionPalette.pNamespaceIDStrings[ paletteNum ] = paletteEntry;
									for( uint32_t id=0; id <numDefaultNamespaceAndBlockIDs; ++id )
									{
										size_t len = strlen( defaultNamespaceAndBlockIDs[id].pNamespaceID );
										if(    len == paletteEntry.size
											&& 0 == memcmp( defaultNamespaceAndBlockIDs[id].pNamespaceID, paletteEntry.pStrNotNullTerminated, len ) )
										{
											sectionPalette.pDefaultBlockIndex[ paletteNum ] = id;
											break;
										}
									}
									++paletteNum;
								}
							}
						}
						if( enkiNBTTAG_End == pStream_->currentTag.tagId && pStream_->level == levelParent + 1 )
						{
							if( pBlocks )
							{
								chunk.countOfSections++;
								assert( sectionPalette.size == 0 ); // a given chunk should use the same format
								chunk.sections[ sectionY ]   = pBlocks;
								chunk.dataValues[ sectionY ] = pData;
								pBlocks = NULL;
								pData   = NULL;
								pBlockStates = NULL;
							}
							if( pBlockStates  && sectionPalette.size )
							{
								chunk.countOfSections++;
								assert( pBlocks == NULL ); // a given chunk should use the same format
								chunk.palette[ sectionY ]  = sectionPalette;
								chunk.sections[ sectionY ] = pBlockStates;
								pBlocks = NULL;
								pData   = NULL;
								pBlockStates = NULL;
								memset( &sectionPalette, 0, sizeof(sectionPalette) );
							}
							++sectionY;
						}
					} while(  pStream_->level > levelParent );
				}

				if( foundXPos && foundZPos && foundSection )
				{
					foundSectionData = 1;
					break;
				}
			}
		}

		if( foundSectionData && chunk.dataVersion )
		{
			// chunk complete
			return chunk;
		}
	}

	if( 0 == foundSectionData )
	{
		// reset to empty as did not find required information
		// memory allocation will be freed when stream freed
		enkiChunkInit( &chunk );
	}
	return chunk;
}

enkiMICoordinate enkiGetChunkOrigin(enkiChunkBlockData * pChunk_)
{
	enkiMICoordinate retVal;
	retVal.x = pChunk_->xPos * ENKI_MI_SIZE_SECTIONS;
	retVal.y = 0;
	retVal.z = pChunk_->zPos * ENKI_MI_SIZE_SECTIONS;
	return retVal;
}

enkiMICoordinate enkiGetChunkSectionOrigin(enkiChunkBlockData * pChunk_, int32_t section_)
{
	enkiMICoordinate retVal;
	retVal.x = pChunk_->xPos * ENKI_MI_SIZE_SECTIONS;
	retVal.y = section_ * ENKI_MI_SIZE_SECTIONS;
	retVal.z = pChunk_->zPos * ENKI_MI_SIZE_SECTIONS;
	return retVal;
}

enkiMIVoxelData enkiGetChunkSectionVoxelData(enkiChunkBlockData * pChunk_, int32_t section_, enkiMICoordinate sectionOffset_)
{
	enkiMIVoxelData retVal;
	retVal.blockID     = 0;
	retVal.dataValue   = 0;
	retVal.paletteIndex = -1;

	uint8_t* pSection    = pChunk_->sections[ section_ ];
	uint32_t paletteSize = pChunk_->palette[ section_ ].size;
	uint32_t posArray    = sectionOffset_.y*ENKI_MI_SIZE_SECTIONS*ENKI_MI_SIZE_SECTIONS + sectionOffset_.z*ENKI_MI_SIZE_SECTIONS + sectionOffset_.x;
	if( paletteSize )
	{
		// size depends on palette
		uint32_t numBits = pChunk_->palette[ section_ ].numBitsPerBlock;


		// Versions prior to 1.16 (DataVersion 2556) have block elements containing values stretching over multiple 64-bit fields.
		// 1.16 and above do not.
		uint32_t blockArrayValue = 0;
		if( pChunk_->dataVersion >= 2556 )
		{
			// do not need to handle bits spread across two uint64_t values
			uint32_t numPer64 = 64 / numBits;
			uint32_t pos64   = posArray / numPer64;
			uint32_t posIn64 = numBits * ( posArray - (pos64 * numPer64) );

			assert( pChunk_->palette[ section_ ].blockArraySize > pos64 );

			uint8_t* pVal64BigEndian = pSection + (8*(size_t)pos64);

			uint64_t val64 =   ( (uint64_t)pVal64BigEndian[0] << 56 ) + ( (uint64_t)pVal64BigEndian[1] << 48 )
							 + ( (uint64_t)pVal64BigEndian[2] << 40 ) + ( (uint64_t)pVal64BigEndian[3] << 32 )
							 + ( (uint64_t)pVal64BigEndian[4] << 24 ) + ( (uint64_t)pVal64BigEndian[5] << 16 )
							 + ( (uint64_t)pVal64BigEndian[6] <<  8 ) + ( (uint64_t)pVal64BigEndian[7]       );

			uint64_t val = val64 >> posIn64;

			uint64_t mask = (~(uint64_t)0) >> (64-numBits);
			uint32_t valmasked = (uint32_t)( val & mask );
			blockArrayValue = valmasked;
			assert( (uint32_t)pChunk_->palette[ section_ ].size > blockArrayValue );
		}
		else
		{

			uint32_t posBits = numBits * posArray;
			uint32_t pos64   = posBits / 64;
			uint32_t posIn64 = posBits - (pos64 * 64);

			assert( pChunk_->palette[ section_ ].blockArraySize > pos64 );

			uint8_t* pVal64BigEndian = pSection + (8*(size_t)pos64);

			uint64_t val64 =   ( (uint64_t)pVal64BigEndian[0] << 56 ) + ( (uint64_t)pVal64BigEndian[1] << 48 )
							 + ( (uint64_t)pVal64BigEndian[2] << 40 ) + ( (uint64_t)pVal64BigEndian[3] << 32 )
							 + ( (uint64_t)pVal64BigEndian[4] << 24 ) + ( (uint64_t)pVal64BigEndian[5] << 16 )
							 + ( (uint64_t)pVal64BigEndian[6] <<  8 ) + ( (uint64_t)pVal64BigEndian[7]       );

			uint64_t val = val64 >> posIn64;

			// handle 'overhang'
			uint32_t maxBitsPossibleIn64Bits = 64 - posIn64;
			uint32_t numBitsIn64      = maxBitsPossibleIn64Bits < numBits ? maxBitsPossibleIn64Bits : numBits;
			uint32_t overhangInNext64 = numBitsIn64 < numBits ? numBits-numBitsIn64 : 0;
				
			uint64_t mask = (~(uint64_t)0) >> (64-numBitsIn64);
			uint32_t valmasked = (uint32_t)( val & mask );

			if( overhangInNext64 )
			{
				uint8_t* pVal64BigEndianPlus1 = pVal64BigEndian + 8;;

				uint64_t val64_2 =  ( (uint64_t)pVal64BigEndianPlus1[0] << 56 ) + ( (uint64_t)pVal64BigEndianPlus1[1] << 48 )
								  + ( (uint64_t)pVal64BigEndianPlus1[2] << 40 ) + ( (uint64_t)pVal64BigEndianPlus1[3] << 32 )
								  + ( (uint64_t)pVal64BigEndianPlus1[4] << 24 ) + ( (uint64_t)pVal64BigEndianPlus1[5] << 16 )
								  + ( (uint64_t)pVal64BigEndianPlus1[6] <<  8 ) + ( (uint64_t)pVal64BigEndianPlus1[7]       );
				uint64_t mask_2      =(~(uint64_t)0) >> (64-overhangInNext64);
				uint64_t val_2       = val64_2;
				uint32_t valmasked_2 = (uint32_t)( val_2 & mask_2 );

				valmasked |= valmasked_2;
			}
			blockArrayValue = valmasked;
			assert( (uint32_t)pChunk_->palette[ section_ ].size > blockArrayValue );
		}

		if( (uint32_t)pChunk_->palette[ section_ ].size > blockArrayValue )
		{
			int32_t index = pChunk_->palette[ section_ ].pDefaultBlockIndex[ blockArrayValue ];
			uint32_t paletteValue = 1; // default to 1, stone
			if( index >= 0 )
			{
				paletteValue = defaultNamespaceAndBlockIDs[index].blockID;
			}
			retVal.blockID = (uint8_t)paletteValue;
			retVal.paletteIndex = (int32_t)blockArrayValue;
		}

		
	}
	else
	{
		uint8_t* pVoxel = pSection + posArray;
		retVal.blockID = *pVoxel;
		if( pChunk_->dataValues[ section_ ] )
		{
			// 4 bit values
			uint32_t posByte = posArray / 2;
			uint32_t offsetByte = 4 * ( posArray - (2*posByte) );
			uint8_t byte = *( pChunk_->dataValues[ section_ ] + posByte );
			retVal.dataValue  = 0xF & ( byte >> offsetByte );
		}
	}
	return retVal;
}

uint8_t enkiGetChunkSectionVoxel( enkiChunkBlockData* pChunk_, int32_t section_, enkiMICoordinate sectionOffset_ )
{
	enkiMIVoxelData voxelData = enkiGetChunkSectionVoxelData( pChunk_, section_, sectionOffset_ );
	return voxelData.blockID;
}

uint32_t* enkiGetMineCraftPalette()
{
	return minecraftPalette;
}

enkiMINamespaceAndBlockIDTable enkiGetNamespaceAndBlockIDTable()
{
	enkiMINamespaceAndBlockIDTable defaultNamespaceAndBlockIDTable;
	defaultNamespaceAndBlockIDTable.namespaceAndBlockIDs    = defaultNamespaceAndBlockIDs;
	defaultNamespaceAndBlockIDTable.size                    = numDefaultNamespaceAndBlockIDs;
	return defaultNamespaceAndBlockIDTable;
}
