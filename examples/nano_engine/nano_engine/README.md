## NanoCanvas8 performance testing

| **Test desc** | **Time(ms)** | **FPS** | **Comments** |
| :-------- |:---:|:---:|:---------|
| Output double buffer   |  17  |  58  |    |
| Clear double buffer    |  15  |  66  |    |
| Template text in non transparent mode  |  20  |  50  |    |
| Template text in transparent mode  |  14  |  71  |    |
| Drawing Flash monochrome bitmap 128x64 in non-transparent mode |  26  |  38  |     |
| Drawing Flash monochrome bitmap 128x64 in transparent mode |  24  |  41  |     |

## Output double buffer

```cpp
bool drawAll()
{
    return true;
}
```

## Clear double buffer

```cpp
bool drawAll()
{
    engine.canvas.clear();
    return false; /* To avoid draw operations */
}
```

## Template text in non transparent mode

```cpp
bool drawAll()
{
    engine.canvas.setMode(0);
    engine.canvas.setColor(RGB_COLOR8(255,0,0));
    engine.canvas.printFixed(0, 15, "This is example of text output", STYLE_NORMAL);
    return false; /* To avoid draw operations */
}
```

## Template text in transparent mode

```cpp
bool drawAll()
{
    engine.canvas.setMode(CANVAS_MODE_TRANSPARENT);
    engine.canvas.setColor(RGB_COLOR8(255,0,0));
    engine.canvas.printFixed(0, 15, "This is example of text output", STYLE_NORMAL);
    return false; /* To avoid draw operations */
}
```

## Drawing Flash monochrome bitmap 128x64 in non-transparent mode

```cpp
bool drawAll()
{
    engine.canvas.setMode(0);
    engine.canvas.setColor(RGB_COLOR8(0,255,255));
    engine.canvas.drawBitmap1(0, 0, 128, 64, Sova);
}
```

## Drawing Flash monochrome bitmap 128x64 in transparent mode

```cpp
bool drawAll()
{
    engine.canvas.setMode(CANVAS_MODE_TRANSPARENT);
    engine.canvas.setColor(RGB_COLOR8(0,255,255));
    engine.canvas.drawBitmap1(0, 0, 128, 64, Sova);
}
```
