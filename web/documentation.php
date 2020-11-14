<?php include("head");?>

<h2>documentation</h2>
<ul>
  <li><a href="#pre">preface</a></li>
  <li><a href="#file">file formats</a>
    <ul>
      <li><a href="#file_namesvalues">raw file format</a></li>
      <li><a href="#file_penn">PennCNV file format</a></li>
    </ul>
  </li>
  <li><a href="#ops">operations</a>
    <ul>
      <li><a href="#ops_singe">single sequence</a>
        <ul>
          <li><a href="#ops_singe_add">add/sub/mul/div</a></li>
          <li><a href="#ops_single_pow">pow/root</a></li>
          <li><a href="#ops_single_blur">blur</a></li>
          <li><a href="#ops_single_trunc">trunc/cut</a></li>
          <li><a href="#ops_single_exp">exp/log/erf</a></li>
          <li><a href="#ops_single_rank">rank</a></li>
          <li><a href="#obs_single_abs">abs</a></li>
          <li><a href="#obs_single_avg">avg</a></li>
          <li><a href="#ops_single_sort">sorting</a></li>
          <li><a href="#obs_single_stripxy">strip XY</a></li>
        </ul>
      </li>
      <li><a href="#ops_dual">two sequences</a>
        <ul>
          <li><a href="#ops_dual_sub">add/sub/mul/div</a></li>
          <li><a href="#ops_dual_sort">sort</a></li>
        </ul>
      </li>
      <li><a href="#ops_global">multiple sequences</a>
        <ul>
          <li><a href="#ops_global_add">add/mul/arithmetic/geometric</a></li>
          <li><a href="#ops_global_median">median</a></li>
          <li><a href="#ops_global_deviation">deviation</a></li>
          <li><a href="#ops_global_align">align</a></li>
        </ul>
      </li>
    </ul>
  </li>
</ul>

<h3 id="pre">preface</h3>
<p>
  <em>noise-free-cnv</em> is a program for analyzing and manipulating DNA
  microarray data (for example from Affymetrix or Illumina platforms).
</p>
<p>
  Besides that, <em>noise-free-cnv</em> can be used to work with sequences of
  numerical values of any type, that are in a meaningful order and that
  consist of data points that are, ideally, distinguishable by names.
  When several sequences with corresponding data points (values with the same
  name) are available, it is possible to compare them to find similarities and
  differences.
</p>
<p>
  The basic working principle when using <em>noise-free-cnv</em> is to load
  sequences of data from files, manipulate them and then either examine the
  result in <em>noise-free-cnv</em> to find irregularities or to save it for
  further use with other programs.
</p>
<h3 id="file">file formats</h3>
<p>
  There are different file formats that <em>noise-free-cnv</em> can handle.
  All of them are text file formats that contain one data point per line. If
  single lines are invalid, they are ignored.
</p>
<dl>
  <dt id="file_namesvalues"><em>raw file format</em></dt>
  <dd>
    <p>
      Each line contains a character string and a decimal value, separated
      by whitespace characters. The decimal separator can either be a
      point or a comma.
    </p>
    <p>
      This is the native file format for <em>noise-free-cnv</em>.
    </p>
<pre class="Frame">
SNP_A-2131660/1/1145994  0.153
SNP_A-1967418/1/2224111  0.2446
SNP_A-1969580/1/2319424  0.0582
SNP_A-4263484/1/2543484  0.3893
SNP_A-1978185/1/2926730  -0.0206
SNP_A-4264431/1/2941694  0.5014
SNP_A-1980898/1/3084986  0.5544
SNP_A-1983139/1/3155127  -0.2811
SNP_A-4265735/1/3292731  0.4831
SNP_A-1995832/1/3695086  0.2174
SNP_A-1995893/1/3710825  -0.0473
SNP_A-1997896/1/3756100  0.3541
SNP_A-1997922/1/3756146  0.0921
SNP_A-2000230/1/4240737  0.169
SNP_A-2000332/1/4243294  -0.0256
SNP_A-4268173/1/4276892  0.1099
SNP_A-2002663/1/4371593  0.1591
SNP_A-2004169/1/4459761  -0.3124
SNP_A-2004249/1/4461025  0.5297
SNP_A-4268681/1/4461905  0.4747
SNP_A-2004332/1/4464544  -0.0218
</pre>
  <p><a href="example.nfcnv">download example file</a></p>
<pre class="Frame">
0  0.000
10  0.174
20  0.342
30  0.500
40  0.643
50  0.766
60  0.866
70  0.940
80  0.985
90  1.0
100  0.985
110  0.940
120  0.866
130  0.766
140  0.643
150  0.500
160  0.342
170  0.174
180  0.000
190  -0.174
200  -0.342
210  -0.500
220  -0.643
230  -0.766
240  -0.866
250  -0.940
260  -0.985
270  -1.0
280  -0.985
290  -0.940
300  -0.866
310  -0.766
320  -0.643
330  -0.500
340  -0.342
350  -0.174
</pre>
  <p><a href="example2.nfcnv">download example file</a></p>
  </dd>
  <dt id="PennCNV"><em>PennCNV file format</em></dt>
  <dd>
    <p>
      The file format used by PennCNV. It has to contain at least the
      columns "Name", "Chr", "Position", ".Log R Ratio" and
      ".B Allele Freq" and is loaded as two sequences, one containing the
      LRR and one containing the BAF values. The names of the data points
      are composed of the "Name", "Chr" and "Position" columns.
    </p>
    <p>
      Accordingly, two sequences are needed to save a PennCNV file. Saving
      only works well if the sequences originate from a PennCNV file, so
      that <em>noise-free-cnv</em> is able to extract the chromosome and
      position number out of the data point's names.
    </p>
<pre class="Frame">
Name	Chr	Position	test.Log R Ratio	test.B Allele Freq
SNP_A-2131660	1	1145994	0.1530	1.0000
SNP_A-1967418	1	2224111	0.2446	1.0000
SNP_A-1969580	1	2319424	0.0582	0.9662
SNP_A-4263484	1	2543484	0.3893	0.9696
SNP_A-1978185	1	2926730	-0.0206	0.0000
SNP_A-4264431	1	2941694	0.5014	0.0341
SNP_A-1980898	1	3084986	0.5544	1.0000
SNP_A-1983139	1	3155127	-0.2811	0.2422
SNP_A-4265735	1	3292731	0.4831	0.0315
SNP_A-1995832	1	3695086	0.2174	0.4495
SNP_A-1995893	1	3710825	-0.0473	0.5547
SNP_A-1997896	1	3756100	0.3541	0.4624
SNP_A-1997922	1	3756146	0.0921	0.0000
SNP_A-2000230	1	4240737	0.1690	0.2961
SNP_A-2000332	1	4243294	-0.0256	0.9852
SNP_A-4268173	1	4276892	0.1099	0.0000
SNP_A-2002663	1	4371593	0.1591	0.0694
SNP_A-2004169	1	4459761	-0.3124	0.9914
SNP_A-2004249	1	4461025	0.5297	0.9839
SNP_A-4268681	1	4461905	0.4747	0.0000
SNP_A-2004332	1	4464544	-0.0218	0.0000
</pre>
  <p><a href="example.lrrbaf">download example file</a></p>
  </dd>
</dl>
<h3 id="ops">operations</h3>
<h4 id="ops_single">single sequence</h4>
<dl>
  <dt id="ops_single_add"><em>add/sub/mul/div</em></dt>
  <dd>
    <p>
      Applies a basic mathematical operation to every data point
      of the sequence, using the value in the entry field.
    </p>
  </dd>
  <dt id="ops_single_pow"><em>pow/root</em></dt>
  <dd>
    <p>
      Applies the pow/root function to every data point of the
      sequence, using the value in the entry field. Imaginary
      components are discarded.
    </p>
  </dd>
  <dt id="ops_single_blur"><em>blur</em></dt>
  <dd>
    <p>
      Applies a low-pass filter to the sequence. In detail, it
      performs a Weierstrass transform with the value in the entry
      field as standard deviation.
    </p>
  </dd>
  <dt id="ops_single_trunc"><em>trunc/cut</em></dt>
  <dd>
    <p>
      All values with a absolute value bigger than the value
      in the entry field are truncated/omitted. All others are
      not changed.
    </p>
  </dd>
  <dt id="ops_single_exp"><em>exp/log/erf</em></dt>
  <dd>
    <p>
      The exponential/natural logarithm/Gauss error function is applied to
      every data point.
    </p>
  </dd>
  <dt id="ops_single_rank"><em>rank</em></dt>
  <dd>
    <p>
      For a perfect normal distribution with a standard deviation of
      sqrt(2), this is the same as <em>erf</em>.
    </p>
  </dd>
  <dt id="ops_single_abs"><em>abs</em></dt>
  <dd>
    <p>
      Every data point is assigned its absolute value.
    </p>
  </dd>
  <dt id="ops_single_avg"><em>avg</em></dt>
  <dd>
    <p>
      The average value of the whole sequence is assigned to every data
      point.
    </p>
  </dd>
  <dt id="ops_single_stripxy"><em>strip XY</em></dt>
  <dd>
    <p>
      Removes data points of the X and Y chromosomes.
    </p>
  </dd>
</dl>
<h4 id="ops_dual">two sequences</h4>
<dl>
  <dt id="ops_dual_sub"><em>add/sub/mul/div</em></dt>
  <dd>
    <p>
      Two sequneces are added/subtracted/multiplied/divided point
      after point. The input sequences have to be in the same order.
      If there are data points missing in any of the sequences,
      <em>noise-free-cnv</em> will skip those (and only those).
    </p>
  </dd>
  <dt id="ops_dual_sort"><em>sort</em></dt>
  <dd>
    <p>
      The data points of the second sequence are sorted so that
      the identifiers are in the same order as in the first
      sequence. Data points with identifiers that are not present
      in both sequences are omitted.
    </p>
  </dd>
</dl>
<h4 id="ops_global">multiple sequences</h4>
<p>
  For these operations, all input sequences have to be in the same
  order. If there are data points missing in some of the sequences,
  <em>noise-free-cnv</em> will skip those (and only those).
</p>
<dl>
  <dt id="ops_global_add"><em>add/mul/arithmetic/geometric</em></dt>
  <dd>
    <p>
      For every data point, the sum/product/arithmetic
      mean/geometric mean throughout all input sequences is
      computed.
    </p>
  </dd>
  <dt id="ops_global_median"><em>median</em></dt>
  <dd>
    <p>
      For every data point, the median mean throughout all input
      sequences is computed. If there is an even number of
      sequences, the arithmetic mean of the upper and lower median
      is used.
    </p>
  </dd>
  <dt id="ops_global_deviation"><em>deviation</em></dt>
  <dd>
    <p>
      For every data point, the standard deviation from zero
      throughout all input sequences is computed. If there is an
      even number of sequences, the arithmetic mean of the upper
      and lower median is used.
    </p>
  </dd>
  <dt id="ops_global_align"><em>align</em></dt>
  <dd>
    <p>
      For every input sequence, one output sequence is generated.
      It contains all the points of the corresponding input
      sequence unchanged that are present in all input sequences.
    </p>
  </dd>
</dl>

<?php include("foot");?>
