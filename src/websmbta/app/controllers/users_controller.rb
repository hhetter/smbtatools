class UsersController < ApplicationController
    before_filter :check_user

  def new
	  @new_user = User.new
	end

	def create
	  @new_user = User.new(params[:user])
    if User.count == 0
      @new_user.admin = true
    end
	  if @new_user.save
      if User.count == 0
        redirect_to log_in_url
      else
        redirect_to :controller => "users", :action => "show"
      end
    else
      render "new"
    end
	end

  def update
    @user = User.update(params[:id],params[:user])
	  if @user.save
	    redirect_to :controller => "config", :action => "general" , :notice => "Signed up!"
	  else
	    render "new"
	  end
  end

  def delete
    user = User.find_by_id(params[:id])
    if user.id != session[:user_id] && User.count != 1
      user.destroy
      redirect_to :controller => "users", :action => "show"
    else
      flash[:error] = "You can't remove your current user or the last user in the database!"
	    redirect_to :action => "show"
    end
  end

  def edit_user
    #Admin-Settings
    @user = User.find_by_id(params[:id])
  end

  def edit_settings
    #Settings for own account
    @user = User.find_by_id(session[:user_id])
    render "edit_user"
  end

  def show
    @all_users = User.find(:all)
  end
end
