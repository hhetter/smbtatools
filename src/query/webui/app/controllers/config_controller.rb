class ConfigController < ApplicationController
  def general
    @host_ip = ""
    @types = Array.new
    @types = ["Inet-Port", "Unix Socket"]
    @port = ""
  end

  def save
    @host_ip = params[:host_ip]
    @type = params[:types]
    @port = params[:port]
    redirect_to :controller => "list", :action => "index"
  end
end
