class ListController < ApplicationController
  require "rexml/document"
  include REXML
  def index
    cmd="/home/benni/run/smbtaquery -h " + $host_ip + " -i " + $port + " -q 'global, list shares;' -x /home/benni/run/list.xml"
    `#{cmd}`
#    `/home/benni/run/smbtaquery -h 10.10.103.116 -i 3491 -q 'global, list shares;' -x /home/benni/run/list.xml`
    @shares = Array.new
    file = File.new( "/home/benni/run/list.xml" )
    doc = Document.new file
    doc.elements.each("smbta_output/list/table_row/table_value[@id='sharename']") {
      |e| @shares << e.text 
    }
  end

end
