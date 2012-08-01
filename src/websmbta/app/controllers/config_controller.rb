class ConfigController < ApplicationController

  before_filter :check_user

  def general
    @dbdrivers = ["pgsql", "mysql", "sqlite"]
    if !(session[:dbdriver] && session[:dbname] && session[:dbuser] && session[:dbhost] )
    session[:dbdriver] = ""
    session[:dbname] = ""
    session[:dbuser] = ""
    session[:dbhost] = ""
    session[:dbpassword] = ""
    end
  end

  def save
    session[:dbdriver] = params[:dbdriver]
    session[:dbname] = params[:dbname]
    session[:dbuser] = params[:dbuser]
    session[:dbhost] = params[:dbhost]
    session[:dbpassword] = params[:dbpassword]
    check_config
    if @check == "\nDatabase connection succesful.\n"
      redirect_to :controller => "overview", :action => "index"
    else
      flash[:error] = "Can't connect to database. Please check your configuration."
      redirect_to  :back
    end
  end

  def check_config
    initial_command
    @cmd += " -t"
    @check = %x[#{@cmd}]
  end

end
