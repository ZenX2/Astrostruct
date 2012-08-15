--
-- Loads the specified fonts into the game for use.
--

-- If we're rendering korean characters, use a korean font!
if (Language == "Korean") then
    LoadFontFace("gui","fonts/dotum.ttf");
else
-- Otherwise we should just use the most massive unicode font I could get my hands on.
    LoadFontFace("gui","fonts/FreeSans.ttf");
end
