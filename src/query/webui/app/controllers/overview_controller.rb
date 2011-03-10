class OverviewController < ApplicationController
  require "rexml/document"
  include REXML
  def index
  end
  def get_domains
    initial_command
    initialize_domains
    render :update do |page|
      page.insert_html :after, "global", :partial => "domains"
      page.call "refreshDomains"
    end
  end
  def refresh_domains
    initial_command
    initialize_domains
    render :update do |page|
      page.replace "domains", :partial => "domains"
    end
  end
  def initialize_domains
    initial_command
    cmd=@cmd + " -q 'global, list domains;' -x /tmp/domains.xml"
    `#{cmd}`
      @domains = Array.new
    file = File.new( "/tmp/domains.xml" )
    doc = Document.new file
    doc.elements.each("smbta_output/list/table_row/table_value[@id='domain']") {
      |e| @domains << e.text
    }
  end
  def initial_command
    if $type == "Inet-Port"
      @cmd="smbtaquery -h " + $host_ip + " -i " + $port
    end
    if $type == "Unix Domain Socket"
      @cmd="smbtaquery -h " + $host_ip + " -u " + $port
    end
  end

end
