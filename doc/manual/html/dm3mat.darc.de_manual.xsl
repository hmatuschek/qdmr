<?xml version="1.0"?>

<xsl:stylesheet version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <xsl:import href="file:///usr/share/xml/docbook/stylesheet/docbook-xsl/html/chunk.xsl"/>

  <xsl:param name="html.stylesheet" select="'/layout.css'"/>
  <xsl:param name="generate.toc">
    appendix  nop
    article   toc,title
    book      toc,title
    chapter   toc
    part      nop
    preface   nop
    qandadiv  nop
    qandaset  nop
    reference toc,title
    section   nop
    set       toc
  </xsl:param>

  <xsl:template name="user.head.content">
    <meta name="viewport" content="width=device-width, initial-scale=1"/>
    <link rel="stylesheet" href="manual.css"/>
    <style type="text/css"><![CDATA[
      #menu-soft { background-color: #4CAF50; }
      #menu-qdmr { background-color: #4CAF50; }
      ]]></style>
  </xsl:template>

  <xsl:template name="body.attributes">
    <xsl:comment>#include virtual="/menu.html" </xsl:comment>
  </xsl:template>

  <xsl:template name="credits.div"/>

  <xsl:template name="footer.navigation">
    <xsl:param name="prev" select="/foo"/>
    <xsl:param name="next" select="/foo"/>
    <xsl:param name="nav.context"/>

    <xsl:variable name="home" select="/*[1]"/>
    <xsl:variable name="up" select="parent::*"/>

    <xsl:if test="count($prev)>0">
      <a id="prev">
        <xsl:attribute name="href">
          <xsl:call-template name="href.target">
            <xsl:with-param name="object" select="$prev"/>
          </xsl:call-template>
        </xsl:attribute>
        Prev
      </a>
    </xsl:if>
    <xsl:if test="count($up) > 0 and generate-id($up) != generate-id($home)">
      <a id="up">
        <xsl:attribute name="href">
          <xsl:call-template name="href.target">
            <xsl:with-param name="object" select="$up"/>
          </xsl:call-template>
        </xsl:attribute>
        Up
      </a>
    </xsl:if>
    <xsl:if test="count($up)>0">
      <a id="index">
        <xsl:attribute name="href">
          <xsl:call-template name="href.target">
            <xsl:with-param name="object" select="$up"/>
          </xsl:call-template>
        </xsl:attribute>
        Index
      </a>
    </xsl:if>
    <xsl:if test="count($next)>0">
      <a id="next">
        <xsl:attribute name="href">
          <xsl:call-template name="href.target">
            <xsl:with-param name="object" select="$next"/>
          </xsl:call-template>
        </xsl:attribute>
        Next
      </a>
    </xsl:if>
  </xsl:template>

  <xsl:template name="chunk-element-content">
  <xsl:param name="prev"/>
  <xsl:param name="next"/>
  <xsl:param name="nav.context"/>
  <xsl:param name="content">
    <xsl:apply-imports/>
  </xsl:param>

  <xsl:call-template name="user.preroot"/>
  
  <html>
    <xsl:call-template name="root.attributes"/>
    <xsl:call-template name="html.head">
      <xsl:with-param name="prev" select="$prev"/>
      <xsl:with-param name="next" select="$next"/>
    </xsl:call-template>

    <body>
      <xsl:call-template name="body.attributes"/>

      <main>
      <!--<nav>
        <xsl:call-template name="user.header.navigation">
          <xsl:with-param name="prev" select="$prev"/>
          <xsl:with-param name="next" select="$next"/>
          <xsl:with-param name="nav.context" select="$nav.context"/>
        </xsl:call-template>

        <xsl:call-template name="header.navigation">
          <xsl:with-param name="prev" select="$prev"/>
          <xsl:with-param name="next" select="$next"/>
          <xsl:with-param name="nav.context" select="$nav.context"/>
        </xsl:call-template>
      </nav>-->

      <xsl:call-template name="user.header.content"/>
      <xsl:copy-of select="$content"/>
      <xsl:call-template name="user.footer.content"/>

      <nav>
        <xsl:call-template name="footer.navigation">
          <xsl:with-param name="prev" select="$prev"/>
          <xsl:with-param name="next" select="$next"/>
          <xsl:with-param name="nav.context" select="$nav.context"/>
        </xsl:call-template>

        <xsl:call-template name="user.footer.navigation">
          <xsl:with-param name="prev" select="$prev"/>
          <xsl:with-param name="next" select="$next"/>
          <xsl:with-param name="nav.context" select="$nav.context"/>
        </xsl:call-template>
      </nav>
      </main>
    </body>
  </html>
  <xsl:value-of select="$chunk.append"/>
</xsl:template>
</xsl:stylesheet>
