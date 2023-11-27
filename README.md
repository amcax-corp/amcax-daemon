# AMCAX-Daemon Introduction

#### Version 1.0.0

AMCAX-Daemon showcases some basic functions that AMCAX geometry kernel supports. There are more functions that can be supported by the kernel but currently not used by Daemon.

Please be informed that this project **does not** come with an AMCAX geometry kernel license, you will need to apply the license on your own.

## Contents

- [AMCAX-Daemon Introduction](#amcax-daemon-introduction)
      - [Version 1.0.0](#version-100)
  - [Contents](#contents)
  - [How to Use](#how-to-use)
    - [User Interfaces](#user-interfaces)
    - [Create / Delete / Move Geometry Objects](#create--delete--move-geometry-objects)
    - [Modify Geometry Objects](#modify-geometry-objects)
    - [Persistent Geometry Objects](#persistent-geometry-objects)
    - [Auxillary Functions](#auxillary-functions)
  - [How to Build](#how-to-build)
    - [Prerequisite](#prerequisite)
    - [Steps](#steps)
  - [Tutorial](#tutorial)
    - [Code Structure Overview](#code-structure-overview)
    - [Create Geometry Objects](#create-geometry-objects)
    - [Modify Geometry Objects](#modify-geometry-objects-1)
    - [Persistent Geometry Objects](#persistent-geometry-objects-1)
    - [Known Issues](#known-issues)
    - [License](#license)
    - [Contact Us](#contact-us)

## How to Use

### User Interfaces

Refer to figures below to locate the operation areas of AMCAX-Daemon

<img width="613" alt="base_tab" src="https://github.com/amcax-kernel/amcax-daemon/assets/150573876/31fe420b-c11a-41ad-8576-ae63361d2757">

 Figure 1 Base Tab

<img width="613" alt="brep_tab" src="https://github.com/amcax-kernel/amcax-daemon/assets/150573876/917655ba-b754-40ac-8217-2ee0d2bb115e">

 Figure 2 BRep Tab

### Create / Delete / Move Geometry Objects

- To create a new geometry object

    Activate **BRep** tab and choose the type of object, and then:
  - For face type: Click on the render area to specify multiple points in sequence, once you are done, press "Enter".
  - For other types: A dialog will pop up, specify the geometry object's parameters in the dialog and click "OK"

- To delete objects, select the objects and press "Delete" key.
- To move objects, select the objects, hold the left mouse button, move to the target point, then release the button

### Modify Geometry Objects

- To edit objects, on the leftside, there's an edit toolbar, choose the kind of operation from the toolbar, pick up objects in sequence to operate on, and then press "Enter" key to apply the operation on selected objects.

### Persistent Geometry Objects

You may either select some objects and save them as file using "Export Selected" function, or save all the objects in your working space using "Save" function.
The objects will be saved in `*.abr` file, which is an AMCAX proprietary format.
You can later load the objects from `*.abr` file into your working space.
All the related functions are availabe under the "File"menu.

### Auxillary Functions

- Change viewpoint

  - Rotate: Hold the right mouse button and move to adjust
  - Pan: Press left/right/up/down key

- Undo/Redo

  Currently only support undo/redo create/delete operation. New features are coming soon.

## How to Build

### Prerequisite

- CMake 3.2.7 +
- Visual Studio 2019

  - Extension: Qt VS Tool (Qt Versions Configured)

- QT 5.15.2

  - Add qmake path (eg. D:\Qt\5.15.2\msvc2019_64\bin) in to PATH environment variable.

- [AMCAX Kernel](https://amcax.net/) 3.2.3

### Steps

- Set environment variables

    **CMAKE\_PREFIX\_PATH=%YOUR\_QT\_INSTALL\_DIR%\\msvc2019%YOUR\_CPU\_ARCH%\\lib\\cmake**

    For example:

    `d:\\qt\\5.15.2\\msvc2019\_64\\lib\\cmake\\`

- Add AMCAX Kernel library

    Unzip the library and put it under the root folder of this project. The folder structure should look like this:

  ```text
      Amcax-Daemon
          |----------AMCAX
          |-------------|-------include
          |-------------|-------lib
          |-------------|-------bin
          |----------Resources
          |----------Src
          |----------...
          |----------CMakeLists.txt
  ```

- Open "Visual Studio 2019" and choose "continue without code", navigate to "File" -> "Open" -> "CMake..." and then open the "CMakeLists.txt" file of this project.

- Build

- Copy the folders ("Debug" & "Release") in AMCAX/bin into your project's output folder.

- Run AmcaxDaemonCore.exe and have fun!

## Tutorial

### Code Structure Overview

The code structure for AMCAX daemon looks like below.

<img width="613" alt="code_structure" src="https://github.com/amcax-kernel/amcax-daemon/assets/150573876/9c2f9a30-537d-4fe5-9c31-3546455b8392">

It"s built upon AMCAX Kernel SDK and OpenGL. From bottom to top:

- "Operation"folder includes helper classes that can call kernel SDK to create the underlying geometry objects, "ObjectDraw" folder includes classes that are used to draw a target object using OpenGL;

- "Object"folder includes the wrapper classes for the kernel's underlying geometry objects, these wrapper classes leverage "Operation" & "ObjectDraw" to support object operation and rendering.

- "Core"folder includes a "DataManager" which manages all the objects that are currently created, "UnRedoHandler" which supports undo/redo operations, "ACAMFile" which supports object persistence to file.

- "Windows" folder contains classes for user interaction and currently leverages libqglviewer for camera/projection related coordinate axes transform.

### Create Geometry Objects

Using AMCAX Kernel to create an object is as simple as:

```cpp
    std::vector<AMCAX::Point3> point_list;
    // TODO:  prepare the points
    std::list<AMCAX::TopoShape> edges;
    for (int i = 1; i < point_list.size(); ++i)
    {
    edges.push_back(AMCAX::MakeEdge(point_list[i-1], point_list[i]));
    }
    edges.push_back(AMCAX::MakeEdge(point_list.back(), (point_list.front())));

    AMCAX::MakeWire mkw;
    mkw.Add(edges);
    AMCAX::TopoShape result = AMCAX::MakeFace(mkw.Wire());
    // TODO: save the result for future use.
```

### Modify Geometry Objects

An example of using AMCAX kernel to fuse multiple objects can be found in the class MultOperate_BoolFuse. It is as simple as:

```cpp
    std::vector<AMCAX::TopoShape> shapeVector;
    // TODO:  prepare the shapes
    AMCAX::TopoShape resultShape = AMCAX::BoolBRepFuse(shapeVector[0], shapeVector[1]);
    for (int i = 2; i < shapeVector.size(); ++i)
    {
        resultShape = AMCAX::BoolBRepFuse(resultShape, shapeVector[i]);
    }
```

More examples can be found in [Operation](./Src/Operation) folder.

### Persistent Geometry Objects

The basic idea is to use AMCAD::TopoCompound and AMCAD::TopoBuilder to compound the objects you want to persistent, and then save them to ABR file. Code snippet as below:

```cpp
    std::string fileName = "foo.abr";
    std::vector<AMCAX::TopoShape> shapeVector;
    //TODO. Prepare the shapes

    AMCAX::TopoCompound comp;
    AMCAX::TopoBuilder build;
    build.MakeCompound(comp);

    std::vector<AMCAX::TopoShape>::iterator it = shapeVector.begin();
    for (; it != shapeVector.end(); it++)
    {
        build.Add(comp, *it);
    }

    AMCAX::ShapeTool::Write(comp, fileName);
```

Refer to [ACAMCoreFile.cpp](./src/core/ACAMCoreFile.cpp) for more details.

### Known Issues

Undo/Redo is not currently supported for actions beyond creating/deleting objects. So if you create an object, move it and then delete it, and "undo" the delete operation, the restored object will not be placed at the moved position.

### License

AMCAX-Daemon uses dual licensing: it is freely available under the terms of the GNU Affero General Public License v3.0 (AGPLv3.0) for open source software development, while commercial applications can apply for a commercial license.

### Contact Us

Welcome to your feedback and contributions!
You can create Git issues here or join our [community](https://bbs.amcax.net/) (Chinese) to further participate into this project.

---

**Copyright © 2023 Hefei Jiushao Intelligent Technology Co., Ltd. All rights reserved.**
