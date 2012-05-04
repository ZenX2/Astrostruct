--
-- Global init file, all these variables will be accessable to the rest of the engine. Know that this file is parsed by lua!
--
Width = 512
Height = 512
-- If you set this to false, make sure to limit your fps as well else you'll max out your cpu and gpu!
VerticalSync = true
-- Set this to 0 to uncap the fps, any other number to limit it.
MaxFPS = 0
-- I don't think this affects speed, but you can change the texture filter here. GL_LINEAR for smooth textures, GL_NEAREST for pixelated textures.
TextureFilter = "GL_LINEAR"
