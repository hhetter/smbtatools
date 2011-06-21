class SessionsController < ApplicationController
  def new
    if User.count == 0
      redirect_to :controller => "users", :action => "new"
    end
  end

  def create
    user = User.authenticate(params[:username], params[:password])
    if user
      session[:user_id] = user.id
      redirect_to :controller => "config", :action => "general", :notice => "Logged in!"
    else
         flash[:error] = "Invalid username or password"
         render "new"
    end
  end

  def destroy
    session[:user_id] = nil
    redirect_to root_url, :notice => "Logged out!"
  end

end
