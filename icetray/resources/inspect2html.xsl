<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		version="1.0">
  <!-- 
       You can make a pretty summary of all available modules and their
       configuration spaces with this stylesheet.
       
       icetray-inspect -x -a (libdir) > summary.xml
       
       xsltproc inspect2html.xsl summary.xml > summary.html
  -->

  <xsl:template match="/icetray-inspect">
    <html>
      <title>Summary of I3Modules and I3ServiceFactories and their parameters</title>
      <style type="text/css">
	body {background-color: #ffffff; color: #000000; }
	body, td, th, h1, h2 {font-family: sans-serif;}
	a:link {color: #000099; text-decoration: none;}
	a:hover {text-decoration: underline;}
	table {
	border-collapse: collapse; width: 600px; cellpadding: 3px; border: 0px; 
	}
	.center {text-align: center;}
	.center table { margin-left: auto; margin-right: auto; text-align: left;}
	.center th { text-align: center; !important }
	td, th { 
	border: 1px solid #000000; font-size: 80%; 
	vertical-align: baseline; padding: 3px
	}
	h1 {font-size: 150%;}
	h2 {font-size: 125%;}
	h5 {font-size: 75%;}
	.p {text-align: left;}
	.e {background-color: #ccccff; font-weight: bold; }
	.h {background-color: #9999cc; font-weight: bold; }
	.g {background-color: #99cc99; }
	.row {background-color: #dddddd; }
	.type {font-weight: normal; font-size: 80%; font-family: monospace; }
	.header {background-color: #999999; font-weight: bold;}
	.module {background-color: #BBBBBB; font-weight: bold;}
      </style>
	<body>
	<div class="center">
	  <h1>Summary of I3Modules and I3ServiceFactories and their parameters</h1>
	  <hr/>
	  <xsl:for-each select="project[module]">
	    <h2> <xsl:value-of select="@name"/> </h2>
	    <a> 
	      <xsl:attribute name="href">
		<xsl:value-of select="url" />
	      </xsl:attribute>         
	      <xsl:value-of select="url" /><xsl:text> at revision </xsl:text>
	      <xsl:value-of select="revision"/>
	    </a>

	    <table>
	      <tr>
		<td class="e" width="30%">Name / Type</td>
		<td class="e" width="50%">Description</td>
		<td class="e" width="20%">Default</td>
	      </tr>
	      <xsl:for-each select="module">
		<tr>
		  <td class="module" colspan="2">
		  <xsl:value-of select="type"/></td>
		  <td class="module" colspan="2">
		  <xsl:value-of select="kind"/></td>
		</tr>
		<tr>
		  <td colspan="3">
		  <pre><xsl:value-of select="description" /></pre></td>
		</tr>
		
		<xsl:for-each select="parameter">
		  <tr class="row">
		    <td class="name" width="30%">
		      <b><xsl:value-of select="name"/></b><br/>
		      <span class="type"><xsl:value-of select="type"/></span>
		    </td>
		    <td class="description" width="50%">
		      <xsl:value-of select="description"/>
		    </td>
		    <td class="default-value" width="20%">
		      <xsl:value-of select="default_value"/>
		    </td>
		  </tr>
		</xsl:for-each>
	      </xsl:for-each>
	    </table>

	    <p/>

	  </xsl:for-each>
	</div>
      </body>
    </html>
  </xsl:template>
</xsl:stylesheet>

