class ConfigController < ApplicationController
  def general
    @dbdrivers = ["pgsql", "mysql", "sqlite"]
    $dbdriver = ""
    $dbname = ""
    $dbuser = ""
    $dbhost = ""
    $dbpassword = ""
  end

  def save
    $dbdriver = params[:dbdriver]
    $dbname = params[:dbname]
    $dbuser = params[:dbuser]
    $dbhost = params[:dbhost]
    $dbpassword = params[:dbpassword]
    redirect_to :controller => "overview", :action => "index"
  end
end
