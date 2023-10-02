$files = Get-Item *.png

$options = "-ml 1" 

foreach($f in $files)
{
    Write-Output $f
    Start-Process -FilePath TextureConverter.exe -ArgumentList $f,$options -Wait
}
pause