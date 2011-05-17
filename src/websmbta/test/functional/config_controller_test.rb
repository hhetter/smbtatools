require 'test_helper'

class ConfigControllerTest < ActionController::TestCase
  test "should get general" do
    get :general
    assert_response :success
  end

end
