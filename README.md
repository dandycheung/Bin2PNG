# bin2png

Convert a binary file to a PNG image and then decrypt it back to binary.

## Usage :

You can build it with Visual Studio or any other C compiler.

Then place your binary file to transform in the same folder as the `bin2png` executable file, and rename it to `in.exe`.

You can then encrypt it to a PNG image :

```shell
bin2png.exe encrypt
```

If successfull, it will then generate an `image.png` file.

You can then decrypt this file back to a binary like this :

```shell
bin2png.exe decrypt
```

It will decrypt the `image.png` file and create a new binary `out.exe`.

## Todo :

- Add parameters to define the name of the binary and image files

Licence
======================
(The MIT License)

Copyright (C) 2013 Leeroy Brun, www.leeroy.me

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

[![Bitdeli Badge](https://d2weczhvl823v0.cloudfront.net/leeroybrun/bin2png/trend.png)](https://bitdeli.com/free "Bitdeli Badge")
