require 'test_helper'

class FunctionsControllerTest < ActionController::TestCase
  test "should get start_function" do
    get :start_function
    assert_response :success
  end

end
