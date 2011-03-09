class ListController < ApplicationController
  require "rexml/document"
  include REXML
  def index
    @global = { "text" => "global", "children"=> "false" }#["text" => domain, "id"=> "domain","children" => "foo"]}
    respond_to do |format|
      format.html
      format.json {
        render :json => @global.to_json
      }
   end
    @users = []
 #   get_domains
 #   get_shares
  end

  def get_domains
    initial_command
    cmd= @cmd + " -q 'global, list domains;' -x /tmp/domains.xml"
    `#{cmd}`
      @domains = Array.new
    file = File.new( "/tmp/domains.xml" )
    doc = Document.new file
    doc.elements.each("smbta_output/list/table_row/table_value[@id='domain']") {
      |e| @domains << e.text
    }
#    File.delete("/tmp/domains.xml")
#    @domains= [{ "text" => "123", "expanded"=> true,"children"=> [{"text" => "test", "id"=> "36"},{"text" => "test2", "id"=> "36"}]}, {"text" => "456"}]
     if @doms != nil
     else
      @doms = Array.new
      @domains.each do |domain|
        @doms << {"text" => domain, "expanded"=> true, "id" => "domain", "children" => "domain"} # , "children"=> true, "id" => "domain"}]
      end
      logger.debug (@doms.to_json)
      respond_to do |format|
        format.json { render :json => @doms.to_json}
      end
    end
  end
  def get_shares
    initial_command
    cmd= @cmd + " -q 'global, list shares;' -x /tmp/shares.xml"
    `#{cmd}`
    @shares = Array.new
    file = File.new( "/tmp/shares.xml" )
    doc = Document.new file
    doc.elements.each("smbta_output/list/table_row/table_value[@id='sharename']") {
      |e| @shares << e.text
    }
    File.delete("/tmp/shares.xml")
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
