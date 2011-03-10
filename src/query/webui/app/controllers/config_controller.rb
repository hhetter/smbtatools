class ConfigController < ApplicationController
  def general
    $host_ip = ""
    @types = Array.new
    @types = ["Inet-Port", "Unix Domain Socket"]
    $type = ""
    $port = ""
  end

  def save
    $host_ip =  params[:host_ip]
    $type = params[:type]
    $port = params[:port]
    redirect_to :controller => "overview", :action => "index"
  end
end
