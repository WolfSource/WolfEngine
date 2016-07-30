Wolf Engine
===========
<p><b>Welcome to the Wolf Engine source code.</b></p> 
<p>The&nbsp;<b>Wolf Engine</b>&nbsp;is the next
generation of&nbsp;<a href="https://persianengine.codeplex.com/">Persian Game Engine</a>&nbsp;which is a
cross-platform open source game engine created by&nbsp;<a href="http://pooyaeimandar.com/">Pooya Eimandar</a>.
Wolf Engine written in C++ and support Python as an embedded script language.
The Wolf used DirectX for rendering on Windows Win32 and Universal Windows Platform, however I’m working on Vulkan, Apple Metal and webGL APIs for Android, OSX/IOS and HTML5 platforms.</p>

<!--What's new-->
<h2><a id="user-content-whatsnew" class="anchor" href="#whatsnew" aria-hidden="true"><svg aria-hidden="true" class="octicon octicon-link" height="16" version="1.1" viewBox="0 0 16 16" width="16"><path d="M4 9h1v1H4c-1.5 0-3-1.69-3-3.5S2.55 3 4 3h4c1.45 0 3 1.69 3 3.5 0 1.41-.91 2.72-2 3.25V8.59c.58-.45 1-1.27 1-2.09C10 5.22 8.98 4 8 4H4c-.98 0-2 1.22-2 2.5S3 9 4 9zm9-3h-1v1h1c1 0 2 1.22 2 2.5S13.98 12 13 12H9c-.98 0-2-1.22-2-2.5 0-.83.42-1.64 1-2.09V6.25c-1.09.53-2 1.84-2 3.25C6 11.31 7.55 13 9 13h4c1.45 0 3-1.69 3-3.5S14.5 6 13 6z"></path></svg></a>What’s new on version 0.2.0.0</h2>
<h3>New Features</h3>

<ul>
<li>Lightweight &quot;<b>GUI framework</b>&quot; added for DirectX
11.X. (includes label, button (with icon), radioButton, checkBox, scrollBar, comboBox, slider, listBox(with icon), image.</li>
<li>Integrated with &quot;<b>Intel TBB build tbb44_20160526oss</b>&quot;</li>
<li>&quot;<b>Blend states</b>&quot; have been added.</li>
<li>Function &quot;<b>w_time_</b><b>::</b><b>from_string</b>&quot;has been added.</li>
<li>Integrated with &quot;<b>ffmpeg</b><b>build &quot;git-78baa45 (2016-04-28)&quot;</b></li>
<li>Integrated with &quot;<b>Python 3.5.1</b>&quot;. Now you can
execute python files directly using “<b>wolf::</b><b>system::w_python</b>” class. please note that Python 3.5.1-amd64 must be installed on your target machine.</li>
<li>Load basic geometries from &quot;<span class=SpellE><b>collada</b>&quot; files.</li>
<li>&quot;<b>Async/Sync</b>&quot;send and received TCP client implemented.</li>
<li>Graphics diagnostic debugging</b>&quot; implemented. Also Wolf prevents from debugging the release application.</li>
</ul>

<h3>Improved</h3>
<ul>
<li>Used &quot;<b>localtime_s</b> instead of <b>deprectaed localtime</b>&quot; in &quot;<b>w_io.h</b>&quot;.</li>
<li>Updated to &quot;<b>Blackmagic DeckLink SDK 10.6.4</b>&quot;.</li>
<li>Improved performance of buffers & textures by using &quot;<b>C++AMP </b>&quot;.</li>
</ul>

<h3>Bug Fixes</h3>
<ul>
</ul>

<!--Projects-->
<h2><a id="user-content-projects" class="anchor" href="#projects" aria-hidden="true"><svg aria-hidden="true" class="octicon octicon-link" height="16" version="1.1" viewBox="0 0 16 16" width="16"><path d="M4 9h1v1H4c-1.5 0-3-1.69-3-3.5S2.55 3 4 3h4c1.45 0 3 1.69 3 3.5 0 1.41-.91 2.72-2 3.25V8.59c.58-.45 1-1.27 1-2.09C10 5.22 8.98 4 8 4H4c-.98 0-2 1.22-2 2.5S3 9 4 9zm9-3h-1v1h1c1 0 2 1.22 2 2.5S13.98 12 13 12H9c-.98 0-2-1.22-2-2.5 0-.83.42-1.64 1-2.09V6.25c-1.09.53-2 1.84-2 3.25C6 11.31 7.55 13 9 13h4c1.45 0 3-1.69 3-3.5S14.5 6 13 6z"></path></svg></a>Projects using Wolf</h2>
<ul>
<li>&quot;<b>Wolf.Playout</b>&quot; is a playout automation software which is used by <a href="http://en.alalam.ir/">Alalam news
network TV</a></li>
<li>&quot;<b>Wolf.CG</b>&quot; is a true real time 3D graphics and character generator which can be integrate
with any newsroom using python API.</li>
</ul>
<p>If you are using Wolf Engine in your projects, kindly let us know about your project.</p>

<!--Branches-->
<h2><a id="user-content-branches" class="anchor" href="#branches" aria-hidden="true"><svg aria-hidden="true" class="octicon octicon-link" height="16" version="1.1" viewBox="0 0 16 16" width="16"><path d="M4 9h1v1H4c-1.5 0-3-1.69-3-3.5S2.55 3 4 3h4c1.45 0 3 1.69 3 3.5 0 1.41-.91 2.72-2 3.25V8.59c.58-.45 1-1.27 1-2.09C10 5.22 8.98 4 8 4H4c-.98 0-2 1.22-2 2.5S3 9 4 9zm9-3h-1v1h1c1 0 2 1.22 2 2.5S13.98 12 13 12H9c-.98 0-2-1.22-2-2.5 0-.83.42-1.64 1-2.09V6.25c-1.09.53-2 1.84-2 3.25C6 11.31 7.55 13 9 13h4c1.45 0 3-1.69 3-3.5S14.5 6 13 6z"></path></svg></a>Branches</h2>
<p>From this repository you can build the Wolf
Engine for Windows, modify them in any way you can imagine, and share your
changes with others. Wolf Engine publishes source in two rolling branches:</p>
<ul>
<li>The <a href="https://github.com/PooyaEimandar/Wolf.Engine/releases">release branch</a>&nbsp;is
extensively tested and makes a great starting point for learning how to use Wolf Engine or making your own real time application or games.</li>
<li>The&nbsp;<a href="https://github.com/PooyaEimandar/Wolf.Engine/tree/master">master branch</a> which is the original source and may be buggy.</li>
</ul>

<!--Building-->
<h2><a id="user-content-building" class="anchor" href="#building" aria-hidden="true"><svg aria-hidden="true" class="octicon octicon-link" height="16" version="1.1" viewBox="0 0 16 16" width="16"><path d="M4 9h1v1H4c-1.5 0-3-1.69-3-3.5S2.55 3 4 3h4c1.45 0 3 1.69 3 3.5 0 1.41-.91 2.72-2 3.25V8.59c.58-.45 1-1.27 1-2.09C10 5.22 8.98 4 8 4H4c-.98 0-2 1.22-2 2.5S3 9 4 9zm9-3h-1v1h1c1 0 2 1.22 2 2.5S13.98 12 13 12H9c-.98 0-2-1.22-2-2.5 0-.83.42-1.64 1-2.09V6.25c-1.09.53-2 1.84-2 3.25C6 11.31 7.55 13 9 13h4c1.45 0 3-1.69 3-3.5S14.5 6 13 6z"></path></svg></a>Building & Running</h2>
<p>The steps below will take you through cloning your own private fork, then compiling and running the Wolf Engine Samples</p>
<h3>Windows</h3>
<ol>
<li>Install&nbsp;<b><a href="https://windows.github.com/">GitHub for Windows</a></b>&nbsp;then&nbsp;<b><a
href="https://guides.github.com/activities/forking/">fork
and clone our repository</a></b>. To use Git from the command line, see the&nbsp;<a
href="https://help.github.com/articles/set-up-git/">Setting up </a>&nbsp;and&nbsp;<a href="https://help.github.com/articles/fork-a-repo/">Fork a Repo</a>&nbsp;articles.</li>
<li>If you'd prefer not to use Git, you can get the source with the clone or press download button and select &quot;<b>Download ZIP</b>&quot;
button on the right, however downloading <a href="https://github.com/PooyaEimandar/Wolf.Engine/releases">latest and stable release one</a> is highly recommended. Based on which type of download you chose; the source compressed file type might be one of the following formats : &quot;<b>.zip</b>&quot; or &quot;<b>.7z</b>&quot;, so make sure use the right tool for decompressing it.</li>
<li>Install&nbsp;<b>Visual Studio 2015</b>. All desktop editions of
Visual Studio 2015 can be used, including&nbsp;<a href="http://www.visualstudio.com/products/visual-studio-community-vs">Visual Studio Community 2015</a>, which is free for small teams and individual developers. Be sure to include C++ support as part of the install, which is disabled by default and also select Windows 10 SDK Kits. Wolf Engine tested on Windows 8.1 and 10 and you need to setup the right Windows SDK kits for your Windows OS. This version of Wolf Engine test on &quot;<b>Windows 10 SDK build 10586.0.</b>&quot;</li>

<li>if you would like to use &quot;<b>blackmagic.decklink</b>&quot;, you need to install <a href="https://www.blackmagicdesign.com/support"> &quot;<b>Blackmagic_Desktop_Video_Windows_10.6.4</b>&quot;</a></li>

<li>If you would like to use Python as scripting language, add &quot;<b>__PYTHON__</b>&quot; preprocessor before building Wolf. Make sure install <a href="https://www.python.org/ftp/python/3.5.1/python-3.5.1-amd64.exe">Python 3.5.1-amd64</a></li>

<li>Open <b>Wolf.Engine.DirectX11_X.sln</b> from &quot;<b>\Wolf.Engine</span>\engine\builds\</b>&quot;
and make sure configuration manager is set to &quot;<b>Debug/x64</b>&quot; or &quot;<b>
Release/x64</b>&quot;. note that Wolf Engine <b>will not build or run</b> on x86 platform</li>
<li>Press <b>F5</b> and build the solution and run the &quot;<b>DirectX11_X.Win32 project</b>&quot;.</li>
<li>Now you can open &quot;<b>Wolf.Engine.Samples.Win32.sln</b>&quot; solution
from &quot;<b>\Wolf.Engine\samples\win</b>&quot; and build samples.</li>
</ol>

<!--Samples-->
<h2><a id="user-content-samples" class="anchor" href="#samples" aria-hidden="true"><svg aria-hidden="true" class="octicon octicon-link" height="16" version="1.1" viewBox="0 0 16 16" width="16"><path d="M4 9h1v1H4c-1.5 0-3-1.69-3-3.5S2.55 3 4 3h4c1.45 0 3 1.69 3 3.5 0 1.41-.91 2.72-2 3.25V8.59c.58-.45 1-1.27 1-2.09C10 5.22 8.98 4 8 4H4c-.98 0-2 1.22-2 2.5S3 9 4 9zm9-3h-1v1h1c1 0 2 1.22 2 2.5S13.98 12 13 12H9c-.98 0-2-1.22-2-2.5 0-.83.42-1.64 1-2.09V6.25c-1.09.53-2 1.84-2 3.25C6 11.31 7.55 13 9 13h4c1.45 0 3-1.69 3-3.5S14.5 6 13 6z"></path></svg></a>Building & Running</h2>
<p>
I've provided a blog on wolfsource.io named <a href="http://wolfsource.io/gpunotes/">GPU Notes</a>, which contains some tutorials for those programmers out there who want to learn gpu programming or need to use WolfSource frameworks such as Wolf Engine on their own projects.
</p> 
<p>
Here are the screenshots of samples, you can read full information about these samples on GPU Notes, just click on the name of sample and you will be redirected to the specific page.
</p>

<!-- Screenshots -->
<table>
  <tr>
    <td>
      <img src="https://raw.githubusercontent.com/PooyaEimandar/Wolf.Engine/0.2.0.0/samples/win/01-Initialize/01-window/view.jpg" alt="01_window/view.jpg" width="256" height="192"/>
      <p><b>Win32</b>: Window</p>
    </td>
    <td>
      <img src="https://raw.githubusercontent.com/PooyaEimandar/Wolf.Engine/0.2.0.0/samples/win/01-Initialize/02-multiple_windows/view.jpg" alt="02_multiple_windows/view.jpg" width="256" height="192"/>
      <p><b>Win32</b>: Multiple Windows</p>
    </td>
    <td>
      <img src="https://raw.githubusercontent.com/PooyaEimandar/Wolf.Engine/0.2.0.0/samples/win/01-Initialize/03-scene/view.jpg" alt="03_scene/view.jpg" width="256" height="192"/>
      <p><b>Win32</b>: Scene</p>
    </td>
  </tr>
  <tr>
    <td>
      <img src="https://raw.githubusercontent.com/PooyaEimandar/Wolf.Engine/0.2.0.0/samples/win/01-Initialize/04-one_scene_multiple_windows/view.jpg" alt="04_one_scene_multiple_windows/view.jpg" width="256" height="192"/>
      <p><b>Win32</b>: One Scene Multiple Windows</p>
    </td>
    <td>
      <img src="https://raw.githubusercontent.com/PooyaEimandar/Wolf.Engine/master/samples/win/01-Initialize/05-multiple_scenes_multiple_windows/view.jpg" alt="05_multiple_scenes_multiple_windows/view.jpg" width="256" height="192"/>
      <p><b>Win32</b>: Multiple Scenes Multiple Windows</p>
    </td>
  </tr>
</table>

<!--Licensing-->
<h2><a id="user-content-licensing" class="anchor" href="#licensing" aria-hidden="true"><svg aria-hidden="true" class="octicon octicon-link" height="16" version="1.1" viewBox="0 0 16 16" width="16"><path d="M4 9h1v1H4c-1.5 0-3-1.69-3-3.5S2.55 3 4 3h4c1.45 0 3 1.69 3 3.5 0 1.41-.91 2.72-2 3.25V8.59c.58-.45 1-1.27 1-2.09C10 5.22 8.98 4 8 4H4c-.98 0-2 1.22-2 2.5S3 9 4 9zm9-3h-1v1h1c1 0 2 1.22 2 2.5S13.98 12 13 12H9c-.98 0-2-1.22-2-2.5 0-.83.42-1.64 1-2.09V6.25c-1.09.53-2 1.84-2 3.25C6 11.31 7.55 13 9 13h4c1.45 0 3-1.69 3-3.5S14.5 6 13 6z"></path></svg></a>Licensing</h2>
<p>
Your access to and use of Wolf Engine on GitHub is governed by the&nbsp;<a href="https://github.com/PooyaEimandar/Wolf.Engine/blob/master/LICENSE">License Agreement</a>. If you don't agree to those terms, then you are not permitted to access or use Wolf Engine.
</p>

<!--Issues-->
<h2><a id="user-content-issues" class="anchor" href="#issues" aria-hidden="true"><svg aria-hidden="true" class="octicon octicon-link" height="16" version="1.1" viewBox="0 0 16 16" width="16"><path d="M4 9h1v1H4c-1.5 0-3-1.69-3-3.5S2.55 3 4 3h4c1.45 0 3 1.69 3 3.5 0 1.41-.91 2.72-2 3.25V8.59c.58-.45 1-1.27 1-2.09C10 5.22 8.98 4 8 4H4c-.98 0-2 1.22-2 2.5S3 9 4 9zm9-3h-1v1h1c1 0 2 1.22 2 2.5S13.98 12 13 12H9c-.98 0-2-1.22-2-2.5 0-.83.42-1.64 1-2.09V6.25c-1.09.53-2 1.84-2 3.25C6 11.31 7.55 13 9 13h4c1.45 0 3-1.69 3-3.5S14.5 6 13 6z"></path></svg></a>Issues</h2>
<p>
Kindly report any issues or post your question
about Wolf Engine on <a href="https://github.com/PooyaEimandar/Wolf.Engine/issues">Issues page</a>. If you have any questions about samples or tutorials, kindly comments on related sample/tutorial page on <a href="http://wolfsource.io/gpunotes/wolfengine">wolfsource.io</a>
</p>

<!--Additional Notes-->
<h2><a id="user-content-notes" class="anchor" href="#notes" aria-hidden="true"><svg aria-hidden="true" class="octicon octicon-link" height="16" version="1.1" viewBox="0 0 16 16" width="16"><path d="M4 9h1v1H4c-1.5 0-3-1.69-3-3.5S2.55 3 4 3h4c1.45 0 3 1.69 3 3.5 0 1.41-.91 2.72-2 3.25V8.59c.58-.45 1-1.27 1-2.09C10 5.22 8.98 4 8 4H4c-.98 0-2 1.22-2 2.5S3 9 4 9zm9-3h-1v1h1c1 0 2 1.22 2 2.5S13.98 12 13 12H9c-.98 0-2-1.22-2-2.5 0-.83.42-1.64 1-2.09V6.25c-1.09.53-2 1.84-2 3.25C6 11.31 7.55 13 9 13h4c1.45 0 3-1.69 3-3.5S14.5 6 13 6z"></path></svg></a>Additional Notes</h2>
<p>
I provided a dedicated blog for programming specially for gpu programming, make sure check out <a href="http://wolfsource.io/gpunotes/">GPU Notes</a> on wolfsource.io.
</p>
<p>
Additionally, in case you are interested, I've written a book with the focus on DirectX 11.1 Game
Programming, you can order it from <a href="http://www.packtpub.com/directx-11-1-game-programming/book">PackT Publishing</a>.
</p>

<!--Road map on Trello-->
<h2><a id="user-content-trello" class="anchor" href="#trello" aria-hidden="true"><svg aria-hidden="true" class="octicon octicon-link" height="16" version="1.1" viewBox="0 0 16 16" width="16"><path d="M4 9h1v1H4c-1.5 0-3-1.69-3-3.5S2.55 3 4 3h4c1.45 0 3 1.69 3 3.5 0 1.41-.91 2.72-2 3.25V8.59c.58-.45 1-1.27 1-2.09C10 5.22 8.98 4 8 4H4c-.98 0-2 1.22-2 2.5S3 9 4 9zm9-3h-1v1h1c1 0 2 1.22 2 2.5S13.98 12 13 12H9c-.98 0-2-1.22-2-2.5 0-.83.42-1.64 1-2.09V6.25c-1.09.53-2 1.84-2 3.25C6 11.31 7.55 13 9 13h4c1.45 0 3-1.69 3-3.5S14.5 6 13 6z"></path></svg></a>Road map</h2>
<a href="https://trello.com/b/O8axM8lj/wolf-engine">Trello</a>

<!--Twitter-->
<h2><a id="user-content-twitter" class="anchor" href="#twitter" aria-hidden="true"><svg aria-hidden="true" class="octicon octicon-link" height="16" version="1.1" viewBox="0 0 16 16" width="16"><path d="M4 9h1v1H4c-1.5 0-3-1.69-3-3.5S2.55 3 4 3h4c1.45 0 3 1.69 3 3.5 0 1.41-.91 2.72-2 3.25V8.59c.58-.45 1-1.27 1-2.09C10 5.22 8.98 4 8 4H4c-.98 0-2 1.22-2 2.5S3 9 4 9zm9-3h-1v1h1c1 0 2 1.22 2 2.5S13.98 12 13 12H9c-.98 0-2-1.22-2-2.5 0-.83.42-1.64 1-2.09V6.25c-1.09.53-2 1.84-2 3.25C6 11.31 7.55 13 9 13h4c1.45 0 3-1.69 3-3.5S14.5 6 13 6z"></path></svg></a>Twitter</h2>
<a href="https://twitter.com/WolfSource_io">@WolfSource_io</a>
