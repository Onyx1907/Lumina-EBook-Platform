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
      "first_login": 1    "OR"     "first_login": 0
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
,
  "5_SET_FAVORITE_GENRES": {
    "client_request": {
      "action": "SET_FAVORITE_GENRES",
      "data": {
        "username": "user123",
        "genres": ["Fiction", "History"]
      }
    },
    "server_response_success": {
      "action": "SET_FAVORITE_GENRES_RESPONSE",
      "status": "SUCCESS",
      "message": ".ژانرهای مورد علاقه با موفقیت ذخیره شدند"
    },
    "server_response_error": {
      "action": "SET_FAVORITE_GENRES_RESPONSE",
      "status": "ERROR",
      "message": "تعداد ژانر باید بین ۱ تا ۳ باشد. یا .خطا در ذخیره ژانرهای مورد علاقه"
    }
  },
  "6_GET_RECOMMENDED_BOOKS": {
    "client_request": {
      "action": "GET_RECOMMENDED_BOOKS",
      "data": {
        "username": "user123"
      }
    },
    "server_response_success": {
      "action": "GET_RECOMMENDED_BOOKS_RESPONSE",
      "status": "SUCCESS",
      "books": [
        {
          "book_id": 1,
          "title": "Book Title",
          "author": "Author Name",
          "cover_base64": "iVBORw0KGgoAAAANSUhEUgAA...",
          "pdf_base64": ""
        }
      ]
    }
  },
  "7_GET_BOOKS_BY_GENRE": {
    "client_request": {
      "action": "GET_BOOKS_BY_GENRE",
      "data": {
        "genre": "Fiction"
      }
    },
    "server_response_success": {
      "action": "GET_BOOKS_BY_GENRE_RESPONSE",
      "status": "SUCCESS",
      "books": [
        {
          "book_id": 2,
          "title": "Another Book",
          "author": "Author Name",
          "price": 15000,
          "cover_base64": "iVBORw0KGgoAAAANSUhEUgAA...",
          "pdf_base64": ""
        }
      ]
    }
  },
  "8_GET_POPULAR_BOOKS": {
    "client_request": {
      "action": "GET_POPULAR_BOOKS"
    },
    "server_response_success": {
      "action": "GET_POPULAR_BOOKS_RESPONSE",
      "status": "SUCCESS",
      "books": [
        {
          "book_id": 3,
          "title": "Popular Book",
          "author": "Author Name",
          "cover_base64": "iVBORw0KGgoAAAANSUhEUgAA...",
          "pdf_base64": ""
        }
      ]
    }
  },
  "9_GET_NEW_BOOKS": {
    "client_request": {
      "action": "GET_NEW_BOOKS"
    },
    "server_response_success": {
      "action": "GET_NEW_BOOKS_RESPONSE",
      "status": "SUCCESS",
      "books": [
        {
          "book_id": 4,
          "title": "New Book",
          "author": "Author Name",
          "cover_base64": "iVBORw0KGgoAAAANSUhEUgAA...",
          "pdf_base64": ""
        }
      ]
    }
  },
  "10_GET_BESTSELLERS": {
    "client_request": {
      "action": "GET_BESTSELLERS"
    },
    "server_response_success": {
      "action": "GET_BESTSELLERS_RESPONSE",
      "status": "SUCCESS",
      "books": [
        {
          "book_id": 5,
          "title": "Bestseller Book",
          "author": "Author Name",
          "cover_base64": "iVBORw0KGgoAAAANSUhEUgAA...",
          "pdf_base64": ""
        }
      ]
    }
  },
  "11_GET_FREE_BOOKS": {
    "client_request": {
      "action": "GET_FREE_BOOKS"
    },
    "server_response_success": {
      "action": "GET_FREE_BOOKS_RESPONSE",
      "status": "SUCCESS",
      "books": [
        {
          "book_id": 6,
          "title": "Free Book",
          "author": "Author Name",
          "price": 0,
          "cover_base64": "iVBORw0KGgoAAAANSUhEUgAA...",
          "pdf_base64": ""
        }
      ]
    }
  },
  "12_GET_PROFILE": {
    "client_request": {
      "action": "GET_PROFILE",
      "data": {
        "username": "user123"
      }
    },
    "server_response_success": {
      "action": "GET_PROFILE_RESPONSE",
      "status": "SUCCESS",
      "profile": {
        "username": "user123",
        "name": "Ali",
        "email": "ali@example.com"
      }
    },
    "server_response_error": {
      "action": "GET_PROFILE_RESPONSE",
      "status": "ERROR",
      "message": ".کاربر یافت نشد"
    }
  },
  "13_UPDATE_PROFILE": {
    "client_request": {
      "action": "UPDATE_PROFILE",
      "data": {
        "username": "user123",
        "name": "Ali Moazed",
        "email": "new_ali@example.com"
      }
    },
    "server_response_success": {
      "action": "UPDATE_PROFILE_RESPONSE",
      "status": "SUCCESS",
      "message": ".اطلاعات حساب کاربری با موفقیت به روزرسانی شد"
    },
    "server_response_error": {
      "action": "UPDATE_PROFILE_RESPONSE",
      "status": "ERROR",
      "message": ".خطا در به روزرسانی اطلاعات حساب کاربری"
    }
  },
  "14_CHANGE_PASSWORD": {
    "client_request": {
      "action": "CHANGE_PASSWORD",
      "data": {
        "username": "user123",
        "old_password": "old_password_123",
        "new_password": "new_password_456"
      }
    },
    "server_response_success": {
      "action": "CHANGE_PASSWORD_RESPONSE",
      "status": "SUCCESS",
      "message": ".رمز عبور با موفقیت تغییر کرد "
    },
    "server_response_error": {
      "action": "CHANGE_PASSWORD_RESPONSE",
      "status": "ERROR",
      "message": ".رمز عبور فعلی اشتباه است یا کاربر یافت نشد"
    }
  },
  "15_GET_PURCHASE_HISTORY": {
    "client_request": {
      "action": "GET_PURCHASE_HISTORY",
      "data": {
        "username": "user123"
      }
    },
    "server_response_success": {
      "action": "GET_PURCHASE_HISTORY_RESPONSE",
      "status": "SUCCESS",
      "history": [
        {
          "purchase_id": 5,
          "book_title": "Book Name",
          "date": "2026-06-29"
        }
      ]
    }
  },
  "16_SEARCH_BOOKS": {
    "client_request": {
      "action": "SEARCH_BOOKS",
      "data": {
        "title": "شاهنامه",
        "author": "فردوسی",
        "publisher_name": "انتشارات طوس"
      }
    },
    "server_response_success": {
      "action": "SEARCH_BOOKS_RESPONSE",
      "status": "SUCCESS",
      "books": [
        {
          "book_id": 10,
          "title": "شاهنامه",
          "author": "فردوسی",
          "publisher_name": "انتشارات طوس",
          "price": 250000,
          "cover_base64": "iVBORw0KGgoAAAANSUhEUgAA...",
          "pdf_base64": ""
        }
      ]
    }
  }
,
  "17_ADD_COMMENT": {
    "client_request": {
      "action": "ADD_COMMENT",
      "data": {
        "book_id": 10,
        "user_id": 42,
        "text": "کتاب بسیار عالی و آموزنده‌ای بود.",
        "rating": 5
      }
    },
    "server_response_success": {
      "action": "ADD_COMMENT_RESPONSE",
      "status": "SUCCESS",
      "message": ".نظر ثبت شد"
    },
    "server_response_error": {
      "action": "ADD_COMMENT_RESPONSE",
      "status": "ERROR",
      "message": ".خطا در ثبت نظر"
    },
    "broadcast_response": {
      "action": "COMMENT_UPDATED",
      "book_id": 10,
      "type": "ADD"
    }
  },
  "18_EDIT_COMMENT": {
    "client_request": {
      "action": "EDIT_COMMENT",
      "data": {
        "comment_id": 154,
        "text": "ویرایش: کتاب خوبی بود اما بخش دوم کمی طولانی بود.",
        "rating": 4
      }
    },
    "server_response_success": {
      "action": "EDIT_COMMENT_RESPONSE",
      "status": "SUCCESS",
      "message": ".نظر ویرایش شد"
    },
    "server_response_error": {
      "action": "EDIT_COMMENT_RESPONSE",
      "status": "ERROR",
      "message": ".خطا در ویرایش"
    },
    "broadcast_response": {
      "action": "COMMENT_UPDATED",
      "comment_id": 154,
      "type": "EDIT"
    }
  },
  "19_DELETE_COMMENT": {
    "client_request": {
      "action": "DELETE_COMMENT",
      "data": {
        "comment_id": 154
      }
    },
    "server_response_success": {
      "action": "DELETE_COMMENT_RESPONSE",
      "status": "SUCCESS",
      "message": ".نظر حذف شد"
    },
    "server_response_error": {
      "action": "DELETE_COMMENT_RESPONSE",
      "status": "ERROR",
      "message": ".خطا در حذف"
    },
    "broadcast_response": {
      "action": "COMMENT_UPDATED",
      "comment_id": 154,
      "type": "DELETE"
    }
  },
  "20_GET_COMMENTS": {
    "client_request": {
      "action": "GET_COMMENTS",
      "data": {
        "book_id": 10
      }
    },
    "server_response_success": {
      "action": "GET_COMMENTS_RESPONSE",
      "status": "SUCCESS",
      "comments": [
        {
          "comment_id": 154,
          "book_id": 10,
          "user_id": 42,
          "text": "کتاب بسیار عالی و آموزنده‌ای بود.",
          "rating": 5
        }
      ]
    }
  }
,
  "21_ADD_TO_CART": {
    "client_request": {
      "action": "ADD_TO_CART",
      "data": {
        "user_id": 42,
        "book_id": 10
      }
    },
    "server_response_success": {
      "action": "ADD_TO_CART_RESPONSE",
      "status": "SUCCESS",
      "message": ".کتاب به سبد خرید اضافه شد"
    },
    "server_response_error": {
      "action": "ADD_TO_CART_RESPONSE",
      "status": "ERROR",
      "message": ".خطا در افزودن کتاب"
    }
  },
  "22_REMOVE_FROM_CART": {
    "client_request": {
      "action": "REMOVE_FROM_CART",
      "data": {
        "user_id": 42,
        "book_id": 10
      }
    },
    "server_response_success": {
      "action": "REMOVE_FROM_CART_RESPONSE",
      "status": "SUCCESS",
      "message": ".کتاب از سبد خرید حذف شد"
    },
    "server_response_error": {
      "action": "REMOVE_FROM_CART_RESPONSE",
      "status": "ERROR",
      "message": ".خطا در حذف کتاب"
    }
  },
  "23_GET_CART": {
    "client_request": {
      "action": "GET_CART",
      "data": {
        "user_id": 42
      }
    },
    "server_response_success": {
      "action": "GET_CART_RESPONSE",
      "status": "SUCCESS",
      "items": [
        {
          "book_id": 10,
          "title": "شاهنامه",
          "price": 250000.0,
          "discount": 10.0
        }
      ],
      "total_price": 250000.0,
      "discount": 25000.0,
      "final_price": 225000.0,
      "count": 1
    }
  },
  "24_FINALIZE_PURCHASE": {
    "client_request": {
      "action": "FINALIZE_PURCHASE",
      "data": {
        "user_id": 42
      }
    },
    "server_response_success": {
      "action": "FINALIZE_PURCHASE_RESPONSE",
      "status": "SUCCESS",
      "message": ".خرید با موفقیت انجام شد"
    },
    "server_response_error": {
      "action": "FINALIZE_PURCHASE_RESPONSE",
      "status": "ERROR",
      "message": ".خطا در نهایی سازی خرید"
    }
  }
}
