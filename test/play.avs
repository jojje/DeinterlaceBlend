LoadPlugin("..\release\DeinterlaceBlend.dll")
# LoadPlugin("..\debug\DeinterlaceBlend.dll")

red   = BlankClip(length=1000, width=1280, height=360, fps=25, color=$FF0000)
green = BlankClip(length=1000, width=1280, height=360, fps=25, color=$00FF00)

# Yellowish interlaced clip. Filter should remove the extreme field aliasing
Interleave( \
  red.subtitle("R"), \
  green.subtitle("G",X=24) \
).assumefieldbased.weave

# ConvertToRGB24()
# ConvertToYUY2()
# ConvertToYV12()
DeinterlaceBlend()
# info()
