<?xml version='1.0'?>
<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:template match="testsuites">
    <html>
      <head>
        <title> gTest Report - <xsl:value-of select="@name"/> </title>
      </head>

      <body bgcolor="#e0e0f0">
        <div align="center">
          <h3 style="border: #008fd5 3px solid">
            <b> gTest Report - <xsl:value-of select="@name"/> </b>
           </h3>
        </div>

        <table cols="5" width="90%" align="center">
          <tr bgcolor="#9573bd">
            <td colspan="5">
              <strong>Unit-test Summary</strong>
            </td>
          </tr>

          <tr bgcolor="#f0e0f0" >
            <td width="30%" style="background-color: #32cd32"> Success APIs </td>
            <td width="20%" style="background-color: #32cd32"> <xsl:value-of select="@successAPIs"/> </td>
            <td width="50%" style="background-color: #32cd32"> success  : <xsl:value-of select="@success"/> </td>
          </tr>
          <tr bgcolor="#f0e0f0" >
            <td width="30%" style="background-color: #ff9090"> Failure APIs </td>
            <td width="20%" style="background-color: #ff9090"> <xsl:value-of select="@failureAPIs"/> </td>
            <td width="50%" style="background-color: #ff9090"> failures : <xsl:value-of select="@failures"/>, disabled : <xsl:value-of select="@disabled"/>, errors : <xsl:value-of select="@errors"/> </td>
          </tr>
          <tr bgcolor="#f0e0f0" >
            <td width="30%" style="background-color: #ba8759"> Total APIs </td>
            <td width="20%" style="background-color: #ba8759"> <xsl:value-of select="@testAPIs"/> </td>
            <td width="50%" style="background-color: #ba8759"> total tests : <xsl:value-of select="@tests"/>, time : <xsl:value-of select="@time"/> sec </td>
          </tr>

            <tr>
              <td colspan="4" style="background-color: #e0e0f0">
                <P></P>
            </td>
          </tr>
        </table>

        <hr align="center" width="90%" color="maroon" />

        <table cols="4" width="90%" align="center">
          <tr>
            <td colspan="4" style="background-color: #e0e0f0">
              <P></P>
            </td>
          </tr>
          <tr bgcolor="#9573bd">
            <td colspan="4"><strong>Result Details</strong></td>
          </tr>
          <tr>
            <td width="25%"> </td>
            <td width="25%"> </td>
            <td width="25%"> </td>
            <td width="25%"> </td>
           </tr>
          <xsl:apply-templates/>
         </table>
        <p/>
         <hr align="center" width="90%" color="maroon" />
      </body>
    </html>
  </xsl:template>

  <xsl:template match="testsuite">
    <tr>
      <td colspan="4" style="background-color: #e0e0f0">
        <P></P>
      </td>
    </tr>
    <tr bgcolor="#8db0d1">
      <td colspan="2">
        Test API
      </td>
      <td colspan="1">
        Test type
      </td>
      <td colspan="1">
        Test purposes
      </td>
    </tr>

    <xsl:apply-templates/>
    <tr bgcolor="#ffffc0">
      <td width="25%">
        tests: <xsl:value-of select="@tests"/>
      </td>
      <td width="25%">
        disabled: <xsl:value-of select="@disabled"/>
      </td>
      <td width="25%">
        failures: <xsl:value-of select="@failures"/>
      </td>
      <td width="25%">
        errors: <xsl:value-of select="@errors"/>
      </td>
    </tr>
  </xsl:template>

  <xsl:template match="testcase">
    <tr bgcolor="#e0f0d0">
      <td colspan="2"> <xsl:value-of select="substring-after(@classname, 'utc_blink_')"/></td>
      <td colspan="1">
        <xsl:value-of select="@name"/>
      </td>
      <xsl:choose>
                <xsl:when test="child::*">
                    <td bgcolor="#ff5050"> Failed </td>
                </xsl:when>
                <xsl:otherwise>
                    <td bgcolor="#50ff50"> Passed </td>
               </xsl:otherwise>
            </xsl:choose>
    </tr>
    <xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="failure">
    <tr><td colspan="4"  bgcolor="#ff9090">
        <table width="100%">
          <tr>
            <th width="15%"> File Name </th>
            <td width="50%" bgcolor="#e0eee0">
              <xsl:value-of select="substring-before(.,':')"/>
            </td>
            <th width="20%"> Line Number </th>
            <td width="10%" bgcolor="#e0eee0">
              <xsl:value-of select='substring-after(substring-before(.,"&#x000A;"),":")'/>
            </td>
          </tr>
          <tr>
            <th width="15%"> message </th>
            <td colspan="3" width="85%" bgcolor="#e0eee0">
              <xsl:value-of select="@message"/>
            </td>
          </tr>
        </table>
    </td></tr>
  </xsl:template>
</xsl:stylesheet>
