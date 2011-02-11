class ListController < ApplicationController
  require "rexml/document"
  include REXML
  def index
    @shares = Array.new
    file = File.new( "/home/benni/run/fritz.xml" )
    doc = Document.new file
    doc.elements.each("smbta_output/list/share") {
      |e| @shares << e.text 
    }
  end

end
