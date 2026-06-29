# قرارداد ارتباطی در پروژه
این سند شامل تمام درخواست‌ها و پاسخ‌های بخش احراز هویت است که به صورت یکجا جمع‌آوری شده است.

---

* **آدرس / متد:** کلاینت پیام JSON زیر را از طریق سوکت ارسال می‌کند.

### درخواست کلاینت و پاسخ‌های سرور:
```json
{
  "1_LOGIN_PROCESS": {
    "client_request": {
      "action": "LOGIN",
      "data": {
        "username": "user123",
        "password": "my_password"
      }
    },
    "server_response_success": {
      "action": "LOGIN_RESPONSE",
      "status": "SUCCESS",
      "message": "!خوش آمدی",
      "user_role": "User"
    },
    "server_response_failed": {
      "action": "LOGIN_RESPONSE",
      "status": "FAILED",
      "message": ".نام کاربری یا رمز عبور اشتباه است یا حساب شما مسدود است"
    }
  },
  "2_REGISTER_PROCESS": {
    "client_request": {
      "action": "REGISTER",
      "data": {
        "username": "new_user",
        "password": "secure_password",
        "role": "User",
        "security_question": "What is your pet's name?",
        "security_answer": "Max"
      }
    },
    "server_response_success": {
      "action": "REGISTER_RESPONSE",
      "status": "SUCCESS",
      "message": ".ثبت نام با موفقیت انجام شد"
    },
    "server_response_username_taken": {
      "action": "REGISTER_RESPONSE",
      "status": "FAILED",
      "message": ".نام کاربری تکراری است"
    },
    "server_response_failed": {
      "action": "REGISTER_RESPONSE",
      "status": "FAILED",
      "message": ".خطا در ثبت نام"
    }
  },
  "3_FORGOT_PASSWORD_STEP_1": {
    "client_request": {
      "action": "FORGOT_PASSWORD",
      "data": {
        "step": "REQUEST_QUESTION",
        "username": "user123"
      }
    },
    "server_response_success": {
      "action": "FORGOT_PASSWORD_RESPONSE",
      "status": "SUCCESS",
      "security_question": "What is your pet's name?"
    },
    "server_response_failed": {
      "action": "FORGOT_PASSWORD_RESPONSE",
      "status": "FAILED",
      "message": ".کاربر یافت نشد"
    }
  },
  "4_FORGOT_PASSWORD_STEP_2": {
    "client_request": {
      "action": "FORGOT_PASSWORD",
      "data": {
        "step": "ANSWER_AND_RESET",
        "username": "user123",
        "security_answer": "Max",
        "new_password": "my_new_password"
      }
    },
    "server_response_success": {
      "action": "FORGOT_PASSWORD_RESPONSE",
      "status": "SUCCESS",
      "message": ".رمز عبور با موفقیت تغییر کرد"
    },
    "server_response_failed": {
      "action": "FORGOT_PASSWORD_RESPONSE",
      "status": "FAILED",
      "message": ".پاسخ امنیتی نادرست است"
    }
  }
}
