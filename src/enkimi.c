
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
	{ "minecraft:air", 0, 0  }, 
	{ "minecraft:stone", 1, 0  }, 
	{ "minecraft:granite", 1, 1  }, 
	{ "minecraft:polished_granite", 1, 2  }, 
	{ "minecraft:diorite", 1, 3  }, 
	{ "minecraft:polished_diorite", 1, 4  }, 
	{ "minecraft:andesite", 1, 5  }, 
	{ "minecraft:polished_andesite", 1, 6  }, 
	{ "minecraft:seagrass", 2, 0  }, 
	{ "minecraft:grass", 2, 0  }, 
	{ "minecraft:grass_block", 2, 0  }, 
	{ "minecraft:dirt", 3, 0  }, 
	{ "minecraft:coarse_dirt", 3, 1  }, 
	{ "minecraft:cobblestone", 4, 0  }, 
	{ "minecraft:oak_planks", 5, 0  }, 
	{ "minecraft:spruce_planks", 5, 1  }, 
	{ "minecraft:birch_planks", 5, 2  }, 
	{ "minecraft:jungle_planks", 5, 3  }, 
	{ "minecraft:acacia_planks", 5, 4  }, 
	{ "minecraft:dark_oak_planks", 5, 5  }, 
	{ "minecraft:oak_sapling", 6, 0  }, 
	{ "minecraft:spruce_sapling", 6, 1  }, 
	{ "minecraft:birch_sapling", 6, 2  }, 
	{ "minecraft:jungle_sapling", 6, 3  }, 
	{ "minecraft:acacia_sapling", 6, 4  }, 
	{ "minecraft:dark_oak_sapling", 6, 5  }, 
	{ "minecraft:bedrock", 7, 0  }, 
	{ "minecraft:flowing_water", 8, 0  }, 
	{ "minecraft:water", 9, 0  }, 
	{ "minecraft:flowing_lava", 10, 0  }, 
	{ "minecraft:lava", 11, 0  }, 
	{ "minecraft:sand", 12, 0  }, 
	{ "minecraft:red_sand", 12, 1  }, 
	{ "minecraft:gravel", 13, 0  }, 
	{ "minecraft:gold_ore", 14, 0  }, 
	{ "minecraft:iron_ore", 15, 0  }, 
	{ "minecraft:coal_ore", 16, 0  }, 
	{ "minecraft:oak_log",       17, 0  }, // up-down
	{ "minecraft:spruce_log",    17, 1  }, // up-down
	{ "minecraft:birch_log",     17, 2  }, // up-down
	{ "minecraft:jungle_log",    17, 3  }, // up-down
	{ "minecraft:oak_log",       17, 4  }, // east-west
	{ "minecraft:spruce_log",    17, 5  }, // east-west
	{ "minecraft:birch_log",     17, 6  }, // east-west
	{ "minecraft:jungle_log",    17, 7  }, // east-west
	{ "minecraft:oak_log",       17, 8  }, // north-south
	{ "minecraft:spruce_log",    17, 9  }, // north-south
	{ "minecraft:birch_log",     17, 10 }, // north-south
	{ "minecraft:jungle_log",    17, 11 }, // north-south 
	{ "minecraft:oak_leaves",    18, 0  }, 
	{ "minecraft:spruce_leaves", 18, 1  }, 
	{ "minecraft:birch_leaves",  18, 2  }, 
	{ "minecraft:jungle_leaves", 18, 3  }, 
	{ "minecraft:oak_leaves",    18, 4  }, // check for decay (if no log within 4 blocks decay)
	{ "minecraft:spruce_leaves", 18, 5  }, // check for decay (if no log within 4 blocks decay)
	{ "minecraft:birch_leaves",  18, 6  }, // check for decay (if no log within 4 blocks decay)
	{ "minecraft:jungle_leaves", 18, 7  }, // check for decay (if no log within 4 blocks decay)
	{ "minecraft:oak_leaves",    18, 8  }, // persistent
	{ "minecraft:spruce_leaves", 18, 9  }, // persistent
	{ "minecraft:birch_leaves",  18, 10 }, // persistent
	{ "minecraft:jungle_leaves", 18, 11 }, // persistent
	{ "minecraft:oak_leaves",    18, 12 }, // persistent (12-15 same as 8-11)
	{ "minecraft:spruce_leaves", 18, 13 }, // persistent (12-15 same as 8-11)
	{ "minecraft:birch_leaves",  18, 14 }, // persistent (12-15 same as 8-11)
	{ "minecraft:jungle_leaves", 18, 15 }, // persistent (12-15 same as 8-11)
	{ "minecraft:sponge",     19, 0  }, 
	{ "minecraft:wet_sponge", 19, 1  }, 
	{ "minecraft:glass", 20, 0  }, 
	{ "minecraft:lapis_ore", 21, 0  }, 
	{ "minecraft:deepslate_lapis_ore", 21, 1  },  // Added after flattening, data value not correct
	{ "minecraft:lapis_block", 22, 0  }, 
	{ "minecraft:dispenser", 23, 0  }, 
	{ "minecraft:sandstone", 24, 0  }, 
	{ "minecraft:note_block", 25, 0  }, 
	{ "minecraft:red_bed", 26, 0  }, 
	{ "minecraft:powered_rail", 27, 0  }, 
	{ "minecraft:detector_rail", 28, 0  }, 
	{ "minecraft:sticky_piston", 29, 0  }, 
	{ "minecraft:cobweb", 30, 0  },
	{ "minecraft:tall_grass", 31, 0  }, 
	{ "minecraft:tall_seagrass", 31, 0  }, 
	{ "minecraft:dead_bush", 32, 0  }, 
	{ "minecraft:piston", 33, 0  }, 
	{ "minecraft:piston_head", 34, 0  }, 
	{ "minecraft:white_concrete", 35, 0  }, 
	{ "minecraft:dandelion", 37, 0  }, 
	{ "minecraft:poppy", 38, 0  }, 
	{ "minecraft:brown_mushroom", 39, 0  }, 
	{ "minecraft:red_mushroom", 40, 0  }, 
	{ "minecraft:gold_block", 41, 0  }, 
	{ "minecraft:iron_block", 42, 0  }, 
	{ "minecraft:smooth_stone_slab", 43, 0  }, 
	{ "minecraft:stone_slab", 44, 0  }, 
	{ "minecraft:brick_wall", 45, 0  }, 
	{ "minecraft:bricks", 45, 0  }, 
	{ "minecraft:tnt", 46, 0  }, 
	{ "minecraft:bookshelf", 47, 0  }, 
	{ "minecraft:mossy_cobblestone", 48, 0  }, 
	{ "minecraft:obsidian", 49, 0  }, 
	{ "minecraft:torch", 50, 0  }, 
	{ "minecraft:fire", 51, 0  }, 
	{ "minecraft:spawner", 52, 0  }, 
	{ "minecraft:oak_stairs", 53, 0  }, 
	{ "minecraft:chest", 54, 0  }, 
	{ "minecraft:redstone_wire", 55, 0  }, 
	{ "minecraft:diamond_ore", 56, 0  }, 
	{ "minecraft:diamond_block", 57, 0  }, 
	{ "minecraft:crafting_table", 58, 0  }, 
	{ "minecraft:wheat", 59, 0  }, 
	{ "minecraft:farmland", 60, 0  }, 
	{ "minecraft:furnace", 61, 0  }, 
	{ "minecraft:campfire", 62, 0  }, 
	{ "minecraft:oak_sign", 63, 0  }, 
	{ "minecraft:oak_door", 64, 0  }, 
	{ "minecraft:ladder", 65, 0  }, 
	{ "minecraft:rail", 66, 0  }, 
	{ "minecraft:stone_stairs", 67, 0  }, 
	{ "minecraft:oak_wall_sign", 68, 0  }, 
	{ "minecraft:lever", 69, 0  }, 
	{ "minecraft:stone_pressure_plate", 70, 0  }, 
	{ "minecraft:iron_door", 71, 0  }, 
	{ "minecraft:oak_pressure_plate", 72, 0  }, 
	{ "minecraft:redstone_ore", 73, 0  }, 
	{ "minecraft:red_concrete", 74, 0  }, 
	{ "minecraft:redstone_wall_torch", 75, 0  }, 
	{ "minecraft:redstone_torch", 76, 0  }, 
	{ "minecraft:stone_button", 77, 0  }, 
	{ "minecraft:snow_block", 78, 0  }, 
	{ "minecraft:ice", 79, 0  }, 
	{ "minecraft:snow", 80, 0  }, 
	{ "minecraft:cactus", 81, 0  }, 
	{ "minecraft:clay", 82, 0  }, 
	{ "minecraft:bamboo", 83, 0  }, 
	{ "minecraft:jukebox", 84, 0  }, 
	{ "minecraft:oak_fence", 85, 0  }, 
	{ "minecraft:pumpkin", 86, 0  }, 
	{ "minecraft:netherrack", 87, 0  }, 
	{ "minecraft:soul_sand", 88, 0  }, 
	{ "minecraft:glowstone", 89, 0  }, 
	{ "minecraft:portal", 90, 0  }, 
	{ "minecraft:carved_pumpkin", 91, 0  }, 
	{ "minecraft:cake", 92, 0  }, 
	{ "minecraft:repeater", 93, 0  }, 
	{ "minecraft:skeleton_skull", 94, 0  }, 
	{ "minecraft:white_stained_glass",      95, 0  }, 
	{ "minecraft:orange_stained_glass",     95, 1  }, 
	{ "minecraft:magenta_stained_glass",    95, 2  }, 
	{ "minecraft:light_blue_stained_glass", 95, 3  }, 
	{ "minecraft:yellow_stained_glass",     95, 4  }, 
	{ "minecraft:lime_stained_glass",       95, 5  }, 
	{ "minecraft:pink_stained_glass",       95, 6  }, 
	{ "minecraft:gray_stained_glass",       95, 7  }, 
	{ "minecraft:light_gray_stained_glass", 95, 8  }, 
	{ "minecraft:cyan_stained_glass",       95, 9  }, 
	{ "minecraft:purple_stained_glass",     95, 10 }, 
	{ "minecraft:blue_stained_glass",       95, 11 }, 
	{ "minecraft:brown_stained_glass",      95, 12 }, 
	{ "minecraft:green_stained_glass",      95, 13 }, 
	{ "minecraft:red_stained_glass",        95, 14 }, 
	{ "minecraft:black_stained_glass",      95, 15 }, 
	{ "minecraft:oak_trapdoor", 96, 0  }, 
	{ "minecraft:turtle_egg", 97, 0  }, 
	{ "minecraft:stone_bricks", 98, 0  }, 
	{ "minecraft:brown_mushroom_block", 99, 0  }, 
	{ "minecraft:red_mushroom_block", 100, 0  }, 
	{ "minecraft:iron_bars", 101, 0  }, 
	{ "minecraft:light_blue_stained_glass_pane", 102, 0  }, 
	{ "minecraft:melon", 103, 0  }, 
	{ "minecraft:pumpkin_stem", 104, 0  }, 
	{ "minecraft:melon_stem", 105, 0  }, 
	{ "minecraft:vine", 106, 0  }, 
	{ "minecraft:oak_fence_gate", 107, 0  }, 
	{ "minecraft:brick_stairs", 108, 0  }, 
	{ "minecraft:stone_brick_stairs", 109, 0  }, 
	{ "minecraft:mycelium", 110, 0  }, 
	{ "minecraft:light_gray_concrete", 111, 0  }, 
	{ "minecraft:nether_brick", 112, 0  }, 
	{ "minecraft:nether_brick_fence", 113, 0  }, 
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
	{ "minecraft:spruce_stairs", 134, 0  }, 
	{ "minecraft:birch_stairs", 135, 0  }, 
	{ "minecraft:jungle_stairs", 136, 0  }, 
	{ "minecraft:command_block", 137, 0  }, 
	{ "minecraft:beacon", 138, 0  }, 
	{ "minecraft:cobblestone_wall", 139, 0  }, 
	{ "minecraft:flower_pot", 140, 0  }, 
	{ "minecraft:carrots", 141, 0  }, 
	{ "minecraft:potatoes", 142, 0  }, 
	{ "minecraft:oak_button", 143, 0  }, 
	{ "minecraft:skeleton_wall_skull", 144, 0  }, 
	{ "minecraft:anvil", 145, 0  }, 
	{ "minecraft:trapped_chest", 146, 0  }, 
	{ "minecraft:light_weighted_pressure_plate", 147, 0  }, 
	{ "minecraft:heavy_weighted_pressure_plate", 148, 0  }, 
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
	{ "minecraft:pink_stained_glass", 159, 0  }, 
	{ "minecraft:white_stained_glass_pane", 160, 0  }, 
	{ "minecraft:acacia_leaves",   161, 0  }, // check for decay (if no log within 4 blocks decay)
	{ "minecraft:dark_oak_leaves", 161, 1  }, // check for decay (if no log within 4 blocks decay)
	{ "minecraft:removed0_leaves", 161, 2  }, // check for decay (if no log within 4 blocks decay)
	{ "minecraft:removed1_leaves", 161, 3  }, // check for decay (if no log within 4 blocks decay)
	{ "minecraft:acacia_leaves",   161, 4  }, // persistent
	{ "minecraft:dark_oak_leaves", 161, 5  }, // persistent
	{ "minecraft:removed0_leaves", 161, 6  }, // persistent
	{ "minecraft:removed1_leaves", 161, 7  }, // persistent
	{ "minecraft:acacia_leaves",   161, 8  }, // persistent (12-15 same as 8-11)
	{ "minecraft:dark_oak_leaves", 161, 9  }, // persistent (12-15 same as 8-11)
	{ "minecraft:removed0_leaves", 161, 10 }, // persistent (12-15 same as 8-11)
	{ "minecraft:removed1_leaves", 161, 11 }, // persistent (12-15 same as 8-11)
	{ "minecraft:oak_wood", 162, 0  }, 
	{ "minecraft:spruce_wood", 162, 1  }, 
	{ "minecraft:birch_wood", 162, 2  }, 
	{ "minecraft:jungle_wood", 162, 3  }, 
	{ "minecraft:acacia_wood", 162, 4  }, 
	{ "minecraft:dark_oak_wood", 162, 5  }, 
	{ "minecraft:crimson_hyphae", 162, 6  }, 
	{ "minecraft:warped_hypha", 162, 7  }, 
	{ "minecraft:stripped_oak_wood", 162, 8  }, 
	{ "minecraft:stripped_spruce_wood", 162, 9  }, 
	{ "minecraft:stripped_birch_wood", 162, 10  }, 
	{ "minecraft:stripped_jungle_wood", 162, 11 }, 
	{ "minecraft:stripped_acacia_wood", 162, 12 }, 
	{ "minecraft:stripped_crimson_hyphae", 162, 14 },  // 
	{ "minecraft:stripped_warped_hyphae", 162, 15 }, 
	{ "minecraft:acacia_stairs", 163, 0  }, 
	{ "minecraft:dark_oak_stairs", 164, 0  }, 
	{ "minecraft:slime_block", 165, 0  }, 
	{ "minecraft:barrier", 166, 0  }, 
	{ "minecraft:iron_trapdoor", 167, 0  }, 
	{ "minecraft:prismarine", 168, 0  }, 
	{ "minecraft:sea_lantern", 169, 0  }, 
	{ "minecraft:hay_block", 170, 0  }, 
	{ "minecraft:white_carpet", 171, 0  }, 
	{ "minecraft:coarse_dirt", 172, 0  }, 
	{ "minecraft:coal_block", 173, 0  }, 
	{ "minecraft:packed_ice", 174, 0  }, 
	{ "minecraft:orange_concrete", 175, 0  }, 
	{ "minecraft:white_banner", 176, 0  }, 
	{ "minecraft:white_wall_banner", 177, 0  }, 
	{ "minecraft:white_concrete_powder", 178, 0  }, 
	{ "minecraft:red_sandstone", 179, 0  }, 
	{ "minecraft:red_sandstone_stairs", 180, 0  }, 
	{ "minecraft:red_sandstone_wall", 181, 0  }, 
	{ "minecraft:red_sandstone_slab", 182, 0  }, 
	{ "minecraft:spruce_fence_gate", 183, 0  }, 
	{ "minecraft:birch_fence_gate", 184, 0  }, 
	{ "minecraft:jungle_fence_gate", 185, 0  }, 
	{ "minecraft:dark_oak_fence_gate", 186, 0  }, 
	{ "minecraft:acacia_fence_gate", 187, 0  }, 
	{ "minecraft:spruce_fence", 188, 0  }, 
	{ "minecraft:birch_fence", 189, 0  }, 
	{ "minecraft:jungle_fence", 190, 0  }, 
	{ "minecraft:dark_oak_fence", 191, 0  }, 
	{ "minecraft:acacia_fence", 192, 0  }, 
	{ "minecraft:spruce_door", 193, 0  }, 
	{ "minecraft:birch_door", 194, 0  }, 
	{ "minecraft:jungle_door", 195, 0  }, 
	{ "minecraft:acacia_door", 196, 0  }, 
	{ "minecraft:dark_oak_door", 197, 0  }, 
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
	{ "minecraft:frosted_ice", 212, 0  }, 
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
	{ "minecraft:gray_concrete", 251, 0  }, 
	{ "minecraft:gray_concrete_powder", 252, 0  }, 
	{ "minecraft:unused_253", 253, 0  }, // pre-flattening unused blockid, added here to ensure have all old id's covered
	{ "minecraft:unused_254", 254, 0  }, // pre-flattening unused blockid, added here to ensure have all old id's covered
	{ "minecraft:structure_block", 255 }
};

static const uint32_t numDefaultNamespaceAndBlockIDs = (uint32_t)sizeof(defaultNamespaceAndBlockIDs)/sizeof(defaultNamespaceAndBlockIDs);

static uint32_t minecraftPaletteIdToBlockIDs[] = {
	0,1,3,6,7,8,9,10,11,12,13,14,15,16,17,18,
	19,20,21,26,27,28,29,30,31,32,33,34,35,36,37,38,
	40,41,42,43,-1,44,45,46,47,48,49,50,51,52,54,55,
	56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,
	72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,
	88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,
	104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,
	120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,
	136,137,138,139,140,141,142,143,144,145,146,147,148,149,150,151,
	152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,
	168,169,170,171,172,173,174,175,176,177,178,179,180,181,182,183,
	184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,
	200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,
	216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,
	232,233,234,235,236,237,238,239,240,241,242,243,244,245,246,247,
	248,249,250,251,252,253,254,255,256,257,258,259,260,261,262,263 };

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

enkiMINamespaceAndBlockIDTable enkiGetMineBlockIDTable()
{
	enkiMINamespaceAndBlockIDTable defaultNamespaceAndBlockIDTable;
	defaultNamespaceAndBlockIDTable.namespaceAndBlockIDs    = defaultNamespaceAndBlockIDs;
	defaultNamespaceAndBlockIDTable.size                    = numDefaultNamespaceAndBlockIDs;
	return defaultNamespaceAndBlockIDTable;
}
