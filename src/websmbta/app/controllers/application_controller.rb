class ApplicationController < ActionController::Base
  protect_from_forgery
  def initial_command
    if session[:dbpassword] != ""
      @cmd="smbtaquery -M " + session[:dbdriver] + " -N " + session[:dbname] + " -S " + session[:dbuser] + " -H " + session[:dbhost] + " -P " + session[:dbpassword] + " -I 0 "
    else
      @cmd="smbtaquery -M " + session[:dbdriver] + " -N " + session[:dbname] + " -S " + session[:dbuser] + " -H " + session[:dbhost] + " -I 0 "
    end
  end
end