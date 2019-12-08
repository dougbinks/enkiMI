Support development of enkiMI through [Github Sponsors](https://github.com/sponsors/dougbinks) or [Patreon](https://www.patreon.com/enkisoftware)

[<img src="https://img.shields.io/static/v1?logo=github&label=Github&message=Sponsor&color=#ea4aaa" width="200"/>](https://github.com/sponsors/dougbinks)    [<img src="https://c5.patreon.com/external/logo/become_a_patron_button@2x.png" alt="Become a Patron" width="150"/>](https://www.patreon.com/enkisoftware)

# enkiMI

## enki Minecraft Importer

A permissively licensed lightweight C and C++ Minecraft Importer library.

## Building

A cmake file is included for the example code, but to use in your project simply add the 2 .c the files in [/src](https://github.com/dougbinks/enkiMI/tree/master/src) into your project and add the director to your include paths.

## Using

enkiMI currently includes NBT file reading (compressed or uncompressed), and region/anvil file chunk extraction.

```
FILE *fp = fopen( "../example/r.1.0.mca", "rb" );
enkiRegionFile regionFile = enkiRegionFileLoad( fp );
for( int i = 0; i < ENKI_MI_REGION_CHUNKS_NUMBER; i++ )
{
    enkiNBTDataStream stream;
    enkiInitNBTDataStreamForChunk( regionFile,  i, &stream );
    if( stream.dataLength )
    {
        enkiChunkBlockData aChunk = enkiNBTReadChunk( &stream );
        enkiMICoordinate chunkOriginPos = enkiGetChunkOrigin( &aChunk ); // y always 0
        printf( "Chunk at xyz{ %d, %d, %d }  Number of sections: %d \n",
            chunkOriginPos.x, chunkOriginPos.y, chunkOriginPos.z, aChunk.countOfSections );
    }
    enkiNBTFreeAllocations( &stream );
}
enkiRegionFileFreeAllocations( &regionFile );
fclose( fp );
```

## Credits

* [Miniz from richgel999](https://github.com/richgel999/miniz)

* Example model [r.1.0.mca Dinner Party](http://www.planetminecraft.com/project/minecraft-custom-modeling-dinner-party/) by Vanillaburp - [Youtube](https://www.youtube.com/user/vanillaburp) - [Twitter @Vanillaburp](https://www.twitter.com/Vanillaburp)

* Thanks to Voxtric for their [C# Minecraft Level Ripper](https://github.com/Voxtric/Minecraft-Level-Ripper) which got us started.

* [ephtracy for the Minecraft palette colours](http://ephtracy.github.io/)

* Thanks to [WesterosCraft](http://www.westeroscraft.com/guide) and [MrBatou](https://www.planetminecraft.com/project/cuirass-dsd-401-by-mrbatou-download-cinematic/) for letting us use their Minecraft constructions to test the importer with our game [Avoyd](https://www.avoyd.com):

![Two Minecraft constructions imported into Avoyd: WesterosCraft King's Landing and MrBatou Cuirassé DSD-401](https://storage.googleapis.com/enkisoftware-images/Avoyd_2017-04-15_Westeroscraft_Kings_Landing_MrBatou_Spaceship.jpg)
[Video: two Minecraft levels imported into Avoyd](https://www.youtube.com/watch?v=cJRFqXPuNe8)

## License (zlib)

Copyright (c) 2017 Juliette Foucaut and Doug Binks

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgement in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.




