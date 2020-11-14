<?php include("head");?>

<h2>About</h2>
<p>
  <em>noise-free-cnv</em> is a program for analyzing and manipulating
  DNA microarray data. It is written in C++, using Gtkmm and FFTW.
</p>
<p>
  <em>noise-free-cnv</em> is free software under the terms of the GNU
  General Public License version 3.
</p>

<h3>Motivation</h3>
<p>
  The <em>noise-free-cnv</em> program package has been specifically developed to
  analyze DNA microarray data for copy number variation and to manipulate such
  datasets in order to reduce noise.
</p>
<p>
  The central program of the <em>noise-free-cnv</em> package is
  <em>noise-free-cnv-gtk</em>, a visual editor for interactive visualization and
  manipulation of DNA microarray data. Besides functioning as a browser for
  direct inspection and verification of alleged CNV findings, it allows the user
  to perform many operations on the data to separate and eliminate noise
  components. 
</p>
<p>
  As a second program, <em>noiser-free-cnv-filter</em> implements a specific
  algorithm for system noise reduction of DNA microarray data. It is usable as a
  command line program to be easily applied to a batch of datasets.
</p>
<p>
  Both programs are able to read and write files suitable for PennCNV.
</p>

<h3>Features</h3>
<ul>
  <li>interactive visualization of DNA microarray data</li>
  <li>provides functions to enhance the quality of the data</li>
  <li>compatible with PennCNV files</li>
  <li>easy to install and to use, no special hardware or software requirements</li>
  <li>available for Windows (XP, Vista, 7) and Linux (Debian, Ubuntu, Mint)</li>
</ul>

<object type="application/x-shockwave-flash" style="width:853px; height:480px;" data="http://www.youtube.com/v/OAGaF8qGVuI?version=3">
<param name="movie" value="http://www.youtube.com/v/OAGaF8qGVuI?version=3" >
<param name="allowFullScreen" value="true" >
<param name="allowscriptaccess" value="always" >
</object>

<object type="application/x-shockwave-flash" style="width:853px; height:480px;" data="http://www.youtube.com/v/VvTbH7suh7c?version=3">
<param name="movie" value="http://www.youtube.com/v/VvTbH7suh7c?version=3" >
<param name="allowFullScreen" value="true" >
<param name="allowscriptaccess" value="always" >
</object>

<?php include("foot");?>
