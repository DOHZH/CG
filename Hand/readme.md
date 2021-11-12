# **Control camera**

![hand.PNG](https://github.com/DOHZH/CG/blob/master/image/hand/hand.PNG?raw=true)

## 1. Properties of camera

We use these 3 values to calculate LookAt Matrix

1. cameraPos: position of camera
2. cameraFront: z-axis direction
3. cameraUp: Up vector

![1.png](https://github.com/DOHZH/CG/blob/master/image/hand/1.png?raw=true)

1. deltaTime, lastTime: control movement speed of camera 
2. fov: Field of View
3. rotationx: rotation of x-axis
4. rotationy: rotation of y-axis
5. lastX, lastY: record initial position of windows, in order to provide a initial position for mouse
   ![2.png](https://github.com/DOHZH/CG/blob/master/image/hand/2.png?raw=true)

## 2. Camera movement

WASD: camera move up/down/right/left

QE: move back and forth

![3.png](https://github.com/DOHZH/CG/blob/master/image/hand/3.png?raw=true)

## 3. From point A to point B

key R

Press backspace to reset the position 

This section to prove the camera can move from A to B, our destination (1,2,3)

![4.png](https://github.com/DOHZH/CG/blob/master/image/hand/4.png?raw=true)

RotationBetweenVectors:  calculate quaternion result of 2 vectors 

![5.png](https://github.com/DOHZH/CG/blob/master/image/hand/5.png?raw=true)

## 4. Rotation

1. manipulate:
   1. right/left rotation: left mouse click + drag
   2. up/down rotation: right mouse click+drag
   3. mixed rotation: press and hold the mouse scroll wheel + drag
      ![6.png](https://github.com/DOHZH/CG/blob/master/image/hand/6.png?raw=true)
2. We use quaternion to realize this function

## 5. Zoom

mouse scroll

![7.png](https://github.com/DOHZH/CG/blob/master/image/hand/7.png?raw=true)

use fov to realize zoom in/out

zoom not change position of camera

![8.png](https://github.com/DOHZH/CG/blob/master/image/hand/8.png?raw=true)

## 6. Reset position of camera

Backspace

Reset position to initial position

![9.png](https://github.com/DOHZH/CG/blob/master/image/hand/9.png?raw=true)

![10.png](https://github.com/DOHZH/CG/blob/master/image/hand/10.png?raw=true)

## 7. Finger movement

1. If you don't do anything, the finger will move by themselves
2. key 1\~9: gesture 1\~9
3. key G: catch
4. key L: gesture "Like"
5. you can move camera and finger at the same time

