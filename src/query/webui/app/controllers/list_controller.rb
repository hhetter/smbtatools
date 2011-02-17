class ListController < ApplicationController
  require "rexml/document"
  include REXML
  def index
    @users = []
    get_domains
    get_shares
  end

  def get_domains
    #debugger
    cmd="smbtaquery -h " + $host_ip + " -i " + $port + " -q 'global, list domains;' -x /tmp/domains.xml"
    `#{cmd}`
    @domains = Array.new
    file = File.new( "/tmp/domains.xml" )
    doc = Document.new file
    doc.elements.each("smbta_output/list/table_row/table_value[@id='domain']") {
      |e| @domains << e.text
    }
    File.delete("/tmp/domains.xml")
  end
  def get_shares
    cmd="smbtaquery -h " + $host_ip + " -i " + $port + " -q 'global, list shares;' -x /tmp/shares.xml"
    `#{cmd}`
    @shares = Array.new
    file = File.new( "/tmp/shares.xml" )
    doc = Document.new file
    doc.elements.each("smbta_output/list/table_row/table_value[@id='sharename']") {
      |e| @shares << e.text
    }
    File.delete("/tmp/shares.xml")
  end
end
