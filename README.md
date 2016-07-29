Wolf Engine
===========
<p><b>Welcome to the Wolf Engine source code.</b></p> 
<p>The&nbsp;<b>Wolf Engine</b>&nbsp;is the next
generation of&nbsp;<a href="https://persianengine.codeplex.com/">Persian Game Engine</a>&nbsp;which is a
cross-platform open source game engine created by&nbsp;<a
href="http://pooyaeimandar.com/">Pooya Eimandar</a>.
Wolf Engine written in C++ and support Python as an embedded script language.
The Wolf used DirectX for rendering on Windows Win32 and Universal Windows Platform, however I’m working on Vulkan, Apple Metal and webGL APIs for Android, OSX/IOS and HTML5 platforms.</p>

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
