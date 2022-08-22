# Goals
- [x] replace the procedurally generated arm holding the mouse with an image
- [x] allow variously sized windows
- [ ] handle secondary monitors nicely
- [ ] allow easy switching between skins
- [ ] allow animating avatars / backgrounds
- [ ] make keyboard arm stretchy as well
- [ ] display precise keypresses (limit to selected window for security)
- [ ] switch skins based on focused window

## Skins
- Each skin is a folder located in `skins/`
- `config.json` is now located with the images, and works on a per-skin basis
- Moving arm image should be prepared so the hand is pointing straight down.

## Config
```json
{
    // size of the window
    "width": 612,
    "height": 467,
    "debug": false, // can help with aligning everything. Toggle with CTRL-D
    "anchor": [162, 110], // position of cat's shoulder
    // where in the image is the pixel attached to anchor
    "armAnchorOffset": [67, 0],
    // area where the avatar is moving it's mouse
    // x, y, width, height, rotation (degrees)
    "catsMouse": [60, 176, 180, 100, 15]
}
```