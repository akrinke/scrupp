# Scrupp

Scrupp is a 2D engine. It consists of a core written in C which uses several SDL libraries and OpenGL to display the content. The core exports functions to the easy to learn Lua programming language. One writes its application using Lua. The result works without changes on Windows, Linux and Mac OS X.

The main aim is to make the core as small and orthogonal as possible. It currently supports:

- Mouse and keyboard control
- Image loading and displaying
- Image compositing using [Cairo](http://cairographics.org/) and the Lua library lua-oocairo by Geoff Richards
- Sound and music
- Movie playback
- TrueType fonts
- Network communication using the Lua library [LuaSocket](http://www.tecgraf.puc-rio.br/~diego/professional/luasocket/) by Diego Nehab
- Loading applications from archives: Just put every ressource in a zip file.

Higher functionalities (e.g. animations) are implemented as plugins in Lua.

Scrupp is free software and is available under the same terms and conditions as the Lua language, the [MIT license](LICENSE.txt).
