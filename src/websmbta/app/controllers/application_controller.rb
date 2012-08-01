class ApplicationController < ActionController::Base

  protect_from_forgery
  helper_method :current_user
  
  def initial_command
    if session[:dbpassword] != ""
      @cmd = "smbtaquery -M " + session[:dbdriver] + " -N " + session[:dbname] + " -S " + session[:dbuser] + " -H " +
        session[:dbhost] + " -P " + session[:dbpassword] + " -I 0 "
    else
      @cmd = "smbtaquery -M " + session[:dbdriver] + " -N " + session[:dbname] + " -S " + session[:dbuser] + " -H " +
        session[:dbhost] + " -I 0 "
    end
  end

  private

  def current_user
    @current_user ||= User.find(session[:user_id]) if session[:user_id]
  end

  def check_user
    if !session[:user_id]
      flash[:error] = "Please log in or contact your administrator!"
      redirect_to log_in_url
    end
  end

  def is_admin
    if session[:user_id] != nil && !User.find_by_id(session[:user_id]).admin
      redirect_to :controller => "overview", :action => "index"
    end
  end
  
end