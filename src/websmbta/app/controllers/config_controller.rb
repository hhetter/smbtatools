class ConfigController < ApplicationController
  before_filter :check_user
  def general
    @dbdrivers = ["pgsql", "mysql", "sqlite"]
    session[:dbdriver] = ""
    session[:dbname] = ""
    session[:dbuser] = ""
    session[:dbhost] = ""
    session[:dbpassword] = ""
  end

  def save
   
    session[:dbdriver] = params[:dbdriver]
    session[:dbname] = params[:dbname]
    session[:dbuser] = params[:dbuser]
    session[:dbhost] = params[:dbhost]
    session[:dbpassword] = params[:dbpassword]
    redirect_to :controller => "overview", :action => "index"
  end

end
