toktx --version

echo convert albedo textures
forfiles /s /m *albedo.png /c ^"cmd /c ^
echo converting @path^&^
toktx --target_type RGB --assign_oetf srgb "albedo.ktx" @path^"

echo convert ao textures
forfiles /s /m *ao.png /c ^"cmd /c ^
echo converting @path^&^
toktx --target_type R --assign_oetf linear "ao.ktx" @path^"

echo convert emissive textures
forfiles /s /m *emissive.png /c ^"cmd /c ^
echo converting @path^&^
toktx --target_type RGB --assign_oetf srgb "emissive.ktx" @path^"

echo convert height textures
forfiles /s /m *height.png /c ^"cmd /c ^
echo converting @path^&^
toktx --target_type R --assign_oetf linear "height.ktx" @path^"

echo convert metal textures
forfiles /s /m *metal.png /c ^"cmd /c ^
echo converting @path^&^
toktx --target_type R --assign_oetf linear "metal.ktx" @path^"

echo convert normal textures
forfiles /s /m *normal.png /c ^"cmd /c ^
echo converting @path^&^
toktx --assign_oetf linear --normalize --normal_mode "normal.ktx" @path^"

echo convert rough textures
forfiles /s /m *rough.png /c ^"cmd /c ^
echo converting @path ^&^
toktx --target_type R --assign_oetf linear "rough.ktx" @path^"

PAUSE