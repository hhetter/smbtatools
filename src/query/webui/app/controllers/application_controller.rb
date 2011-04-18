class ApplicationController < ActionController::Base
  protect_from_forgery
  def initial_command
      @cmd="smbtaquery -M " + $dbdriver + " -N " + $dbname + " -S " + $dbuser + " -H " + $dbhost + " -P " + $dbpassword + " -I 0 "
  end
end