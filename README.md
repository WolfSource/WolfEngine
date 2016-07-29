Wolf Engine
===========
<p><b>Welcome to the Wolf Engine source code.</b></p> 
<p>The&nbsp;<b>Wolf Engine</b>&nbsp;is the next
generation of&nbsp;<a href="https://persianengine.codeplex.com/" target="_blank">Persian Game Engine</a>&nbsp;which is a
cross-platform open source game engine created by&nbsp;<a href="http://pooyaeimandar.com/" target="_blank">Pooya Eimandar</a>.
Wolf Engine written in C++ and support Python as an embedded script language.
The Wolf used DirectX for rendering on Windows Win32 and Universal Windows Platform, however I’m working on Vulkan, Apple Metal and webGL APIs for Android, OSX/IOS and HTML5 platforms.</p>

<!--What's new-->
<h2><a id="user-content-whatsnew" class="anchor" href="#whatsnew" aria-hidden="true"><svg aria-hidden="true" class="octicon octicon-link" height="16" version="1.1" viewBox="0 0 16 16" width="16"><path d="M4 9h1v1H4c-1.5 0-3-1.69-3-3.5S2.55 3 4 3h4c1.45 0 3 1.69 3 3.5 0 1.41-.91 2.72-2 3.25V8.59c.58-.45 1-1.27 1-2.09C10 5.22 8.98 4 8 4H4c-.98 0-2 1.22-2 2.5S3 9 4 9zm9-3h-1v1h1c1 0 2 1.22 2 2.5S13.98 12 13 12H9c-.98 0-2-1.22-2-2.5 0-.83.42-1.64 1-2.09V6.25c-1.09.53-2 1.84-2 3.25C6 11.31 7.55 13 9 13h4c1.45 0 3-1.69 3-3.5S14.5 6 13 6z"></path></svg></a>What’s new on version 0.2.0.0</h2>
<h3>New Features</h3>

<ul>
<li>Lightweight &quot;<b>GUI framework</b>&quot; added for DirectX
11.X. (includes label, button (with icon), radioButton, checkBox, scrollBar, comboBox, slider, listBox(with icon), image.</li>
<li>Integrated with &quot;<b>Intel TBB build tbb44_20160526oss</b>&quot;</li>
<li>Blend states</b>&quot; have been added.</li>
<li>Function &quot;<b>w_time_</b><b>::</b><b>from_string</b>&quot;has been added.</li>
<li>Integrated with &quot;<b>ffmpeg</b><b>build &quot;git-78baa45 (2016-04-28)&quot;</b></li>
<li>Integrated with &quot;<b>Python 3.5.1</b>&quot;. Now you can
execute python files directly using “<b>wolf::</b><b>system::w_python</b>” class. please note that Python 3.5.1 x64 must be installed on your target machine.</li>
<li>Load basic geometries from &quot;<span class=SpellE><b>collada</b><b>files</b>&quot;.</li>
<li>&quot;<b>Async/Sync</b>&quot;send and received TCP client implemented.</li>
<li>Graphics diagnostic debugging</b>&quot; implemented. Also Wolf prevents from debugging the release application.</li>
</ul>

<h3>Improved</h3>
<ul>
<li>Used &quot;<b>localtime_s</b> instead of <b>deprectaed localtime</b>&quot; in w_io.h.</li>
<li>Updated to &quot;<b>Blackmagic</b><b>DeckLink SDK 10.6.4</b>&quot;.</li>
</ul>

<h3>Buf Fixes</h3>
<ul>
</ul>

<!--Projects-->
<h2><a id="user-content-projects" class="anchor" href="#projects" aria-hidden="true"><svg aria-hidden="true" class="octicon octicon-link" height="16" version="1.1" viewBox="0 0 16 16" width="16"><path d="M4 9h1v1H4c-1.5 0-3-1.69-3-3.5S2.55 3 4 3h4c1.45 0 3 1.69 3 3.5 0 1.41-.91 2.72-2 3.25V8.59c.58-.45 1-1.27 1-2.09C10 5.22 8.98 4 8 4H4c-.98 0-2 1.22-2 2.5S3 9 4 9zm9-3h-1v1h1c1 0 2 1.22 2 2.5S13.98 12 13 12H9c-.98 0-2-1.22-2-2.5 0-.83.42-1.64 1-2.09V6.25c-1.09.53-2 1.84-2 3.25C6 11.31 7.55 13 9 13h4c1.45 0 3-1.69 3-3.5S14.5 6 13 6z"></path></svg></a>Projects using Wolf</h2>
<ul>
<li>&quot;<b>Wolf.Playout</b>&quot; is a playout automation software which is used by <a href="http://en.alalam.ir/" target="_blank">Alalam news
network TV</a></li>
<li>&quot;<b>Wolf.CG</b>&quot; is a true real time 3D graphics and character generator which can be integrate
with any newsroom using python API.</li>
</ul>
<p>If you are using Wolf Engine in your projects, kindly let us know about it.</p>

<!--Branches-->
<h2><a id="user-content-branches" class="anchor" href="#branches" aria-hidden="true"><svg aria-hidden="true" class="octicon octicon-link" height="16" version="1.1" viewBox="0 0 16 16" width="16"><path d="M4 9h1v1H4c-1.5 0-3-1.69-3-3.5S2.55 3 4 3h4c1.45 0 3 1.69 3 3.5 0 1.41-.91 2.72-2 3.25V8.59c.58-.45 1-1.27 1-2.09C10 5.22 8.98 4 8 4H4c-.98 0-2 1.22-2 2.5S3 9 4 9zm9-3h-1v1h1c1 0 2 1.22 2 2.5S13.98 12 13 12H9c-.98 0-2-1.22-2-2.5 0-.83.42-1.64 1-2.09V6.25c-1.09.53-2 1.84-2 3.25C6 11.31 7.55 13 9 13h4c1.45 0 3-1.69 3-3.5S14.5 6 13 6z"></path></svg></a>Branches</h2>
<p>From this repository you can build the Wolf
Engine for Windows, modify them in any way you can imagine, and share your
changes with others. Wolf Engine publishes source in two rolling branches:</p>
<ul>
<li>The <a href="https://github.com/PooyaEimandar/Wolf.Engine/releases" target="_blank">release branch</a>&nbsp;is
extensively tested and makes a great starting point for learning how to use Wolf Engine or making your own real time application or games.</li>
<li>The&nbsp;<a href="https://github.com/PooyaEimandar/Wolf.Engine/tree/master" target="_blank">master branch</a> which is the original source and may be buggy and it may not even compile.</li>
</ul>

<!--Building-->
<h2><a id="user-content-building" class="anchor" href="#building" aria-hidden="true"><svg aria-hidden="true" class="octicon octicon-link" height="16" version="1.1" viewBox="0 0 16 16" width="16"><path d="M4 9h1v1H4c-1.5 0-3-1.69-3-3.5S2.55 3 4 3h4c1.45 0 3 1.69 3 3.5 0 1.41-.91 2.72-2 3.25V8.59c.58-.45 1-1.27 1-2.09C10 5.22 8.98 4 8 4H4c-.98 0-2 1.22-2 2.5S3 9 4 9zm9-3h-1v1h1c1 0 2 1.22 2 2.5S13.98 12 13 12H9c-.98 0-2-1.22-2-2.5 0-.83.42-1.64 1-2.09V6.25c-1.09.53-2 1.84-2 3.25C6 11.31 7.55 13 9 13h4c1.45 0 3-1.69 3-3.5S14.5 6 13 6z"></path></svg></a>Building & Running</h2>
<p>The steps below will take you through cloning your own private fork, then compiling and running the Wolf Engine Samples</p>
<h3>Windows</h3>
<ol>
<li>Install&nbsp;<b><a href="https://windows.github.com/" target="_blank">GitHub for Windows</a></b>&nbsp;then&nbsp;<b><a
href="https://guides.github.com/activities/forking/" target="_blank">fork
and clone our repository</a></b>. To use Git from the command line, see the&nbsp;<a
href="https://help.github.com/articles/set-up-git/" target="_blank">Setting up </a>&nbsp;and&nbsp;<a href="https://help.github.com/articles/fork-a-repo/" target="_blank">Fork a Repo</a>&nbsp;articles.</li>
<li>If you'd prefer not to use Git, you can get the source with the Clone or press download button and select &quot;<b>Download ZIP</b>&quot;
button on the right, however downloading <a href="https://github.com/PooyaEimandar/Wolf.Engine/releases" target="_blank">latest and stable release one</a> is highly recommended. Based on which type of download you chose; the source compressed file might be one of following formats : &quot;<b>.zip</b>&quot; or &quot;<b>.7z</b>&quot;, so make sure use the right tool for decompressing it.</li>
<li>Install&nbsp;<b>Visual Studio 2015</b>. All desktop editions of
Visual Studio 2015 can be used, including&nbsp;<a href="http://www.visualstudio.com/products/visual-studio-community-vs" target="_blank">Visual Studio Community 2015</a>, which is free for small teams and individual developers. Be sure to include C++ support as part of the install, which is disabled by default and also select Windows 10 SDK Kits. Wolf Engine tested on Windows 8.1 and 10 and you need to setup the right Windows SDK kits for your Windows OS. This version of Wolf Engine test on &quot;<b>Windows 10 SDK build 10586.0.</b>&quot;</li>
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
I've provided a blog on wolfsource.io named <a href="http://wolfsource.io/gpunotes/" target="_blank">GPU Notes</a>, which contains some tutorials for those programmers out there who want to learn gpu programming or need to use WolfSource frameworks such as Wolf engine on their own projects.
</p> 
<p>
Here are the screenshots of samples, you can read full information about these sample on GPU Notes, just click on image and you will be redirected to the page
</p>
