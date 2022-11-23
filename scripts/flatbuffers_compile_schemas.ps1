####
#   This script helps generate FlatBuffers serialization functions starting from the schema.
#   First you need to build FlatBuffers project inside the submodule.
####

$ProjectPath = $PSScriptRoot + '\..'

$PreviousLocation = Get-Location


Set-Location "$ProjectPath\src\io\flatbuffers"

$FlatcPath = "$ProjectPath\third_party\flatbuffers\out\build\x64-Debug\flatc"

Start-Process -NoNewWindow -PassThru -FilePath $FlatcPath -ArgumentList "--cpp $ProjectPath\flatbuffers\tile.fbs"

Start-Process -NoNewWindow -PassThru -FilePath $FlatcPath -ArgumentList "--cpp $ProjectPath\flatbuffers\tileset.fbs"

Start-Process -NoNewWindow -PassThru -FilePath $FlatcPath -ArgumentList "--cpp $ProjectPath\flatbuffers\gamemap.fbs"


Set-Location $PreviousLocation