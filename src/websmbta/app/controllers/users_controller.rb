class UsersController < ApplicationController
  def new
	  @user = User.new
	end

	def create
	  @user = User.new(params[:user])
	  if @user.save
	    redirect_to :controller => "config", :action => "general" , :notice => "Signed up!"
	  else
	    render "new"
	  end
	end

  def update
    @user = User.update(params[:id],params[:user])
    #@user.update_attributes(password_hash, username)
	  if @user.save
	    redirect_to :controller => "config", :action => "general" , :notice => "Signed up!"
	  else
	    render "new"
	  end
  end

  def delete
    user = User.find_by_id(params[:id])
    user.destroy
    redirect_to :controller => "users", :action => "show"
  end

  def edit_user
    @user = User.find_by_id(params[:id])
  end

  def show
    @all_users = User.find(:all)
  end
end
