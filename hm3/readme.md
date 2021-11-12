# Normal&parallax mapping

## 1. Some intro

1. use glad, stb_image, etc. You can find them in the document "includes"
2. texture in documents "textures"
3. in x64 compile, windows. Please use vs compile in debug mode, and release mode can't load header files properly
4. includes/myself: some class written by myself

## 2. Manipulate

1. View:

   1. key WASD: up/left/down/right

      ![1.png](https://github.com/DOHZH/CG/blob/master/image/hm3/1.png?raw=true)

   2. mouse scroll: zoom in/out

      ![2.png](https://github.com/DOHZH/CG/blob/master/image/hm3/2.png?raw=true)

   3. key up/down/left/right: control light position

   4. key U/P: control light forward and backward

   5. I decelerate the speed, please hold this key long to check

      ![3.png](https://github.com/DOHZH/CG/blob/master/image/hm3/3.png?raw=true)

      ![4.png](https://github.com/DOHZH/CG/blob/master/image/hm3/4.png?raw=true)

## 3.Parallax Mapping

1. Add height info in normal mapping

2. program set the initial height = 0.

3. Manipulate: key Q/E

   1. change height to reduce/plus effect of parallax mapping
   2. Q: reduce, min height = 0
   3. E: plus, max height = 1

4. textures/disp_1.png: displacement mapping

    ![5.png](https://github.com/DOHZH/CG/blob/master/image/hm3/5.png?raw=true)

   ![6.png](https://github.com/DOHZH/CG/blob/master/image/hm3/6.png?raw=true)

