class UsersController < ApplicationController
  def new
	  @user = User.new
	end

	def create
	  @user = User.new(params[:user])
    debugger
	  if @user.save
	    redirect_to :controller => "config", :action => "general" , :notice => "Signed up!"
	  else
	    render "new"
	  end
	end
  
end
