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
      "user_role": "User",
      "first_login": 1    "OR"     "first_login": 0,
      "user_id" : 11   
    },

     *****ADMIN*****
{
    "action": "LOGIN_RESPONSE",
    "status": "SUCCESS",
    "message": "!خوش آمدی مدیر",
    "user_role": "Admin",
    "first_login": 0,
    "user_id": -1
}
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
        "username": "user42",
        "genres": ["Epic", "History"]
      }
    },
    "server_response_success": {
      "action": "SET_FAVORITE_GENRES_RESPONSE",
      "status": "SUCCESS",
      "message": ".ژانرهای مورد علاقه با موفقیت ذخیره شدند"
    },
    "server_response_error_validation": {
      "action": "SET_FAVORITE_GENRES_RESPONSE",
      "status": "ERROR",
      "message": ".تعداد ژانر باید بین ۱ تا ۳ باشد"
    },
    "server_response_error_db": {
      "action": "SET_FAVORITE_GENRES_RESPONSE",
      "status": "ERROR",
      "message": ".خطا در ذخیره ژانرهای مورد علاقه"
    }
  },
  "6_GET_RECOMMENDED_BOOKS": {
    "client_request": {
      "action": "GET_RECOMMENDED_BOOKS",
      "data": {
        "user_id": 42
      }
    },
    "server_response_success": {
      "action": "GET_RECOMMENDED_BOOKS_RESPONSE",
      "status": "SUCCESS",
      "books": [
        {
          "id": 10,
          "title": "شاهنامه",
          "author": "فردوسی",
          "genre": "Epic",
          "price": 250000.0,
          "discount_percentage": 10.0,
          "cover_image_path": "/home/username/BookClub_Storage/shahnameh.jpg"
        }
      ]
    }
  },
  "7_GET_BOOKS_BY_GENRE": {
    "client_request": {
      "action": "GET_BOOKS_BY_GENRE",
      "data": {
        "genre": "Epic"
      }
    },
    "server_response_success": {
      "action": "GET_BOOKS_BY_GENRE_RESPONSE",
      "status": "SUCCESS",
      "books": [
        {
          "id": 10,
          "title": "شاهنامه",
          "author": "فردوسی",
          "genre": "Epic",
          "price": 250000.0,
          "discount_percentage": 10.0,
          "cover_image_path": "/home/username/BookClub_Storage/shahnameh.jpg",
          "publisher_name": "انتشارات فردوسی"
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
          "id": 10,
          "title": "شاهنامه",
          "author": "فردوسی",
          "genre": "Epic",
          "price": 250000.0,
          "discount_percentage": 10.0,
          "cover_image_path": "/home/username/BookClub_Storage/shahnameh.jpg"
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
          "id": 10,
          "title": "شاهنامه",
          "author": "فردوسی",
          "genre": "Epic",
          "price": 250000.0,
          "discount_percentage": 10.0,
          "cover_image_path": "/home/username/BookClub_Storage/shahnameh.jpg"
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
          "id": 10,
          "title": "شاهنامه",
          "author": "فردوسی",
          "genre": "Epic",
          "price": 250000.0,
          "discount_percentage": 10.0,
          "cover_image_path": "/home/username/BookClub_Storage/shahnameh.jpg"
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
          "id": 12,
          "title": "دیوان حافظ",
          "author": "حافظ",
          "genre": "Poetry",
          "price": 0.0,
          "discount_percentage": 0.0,
          "cover_image_path": "/home/username/BookClub_Storage/shahnameh.jpg"
        }
      ]
    }
  },
  "12_GET_PROFILE": {
    "client_request": {
      "action": "GET_PROFILE",
      "data": {
        "user_id": 42
      }
    },
    "server_response_success": {
      "action": "GET_PROFILE_RESPONSE",
      "status": "SUCCESS",
      "profile": {
        "name": "فرهاد احمدی",
        "email": "farhad@example.com",
        "favorite_genres": ["Epic", "History"],
        "total_purchases": 5
      }
    },
    "server_response_error": {
      "action": "GET_PROFILE_RESPONSE",
      "status": "ERROR",
      "message": ".کاربر یافت نشد"
    }
  },"13_UPDATE_PROFILE": {
  "client_request": {
    "action": "UPDATE_PROFILE",
    "data": {
      "user_id": 42,
      "username": "user42_new",
      "name": "فرهاد احمدی علوی",
      "email": "farhad_new@example.com"
    }
  },
  "server_response_success": {
    "action": "UPDATE_PROFILE_RESPONSE",
    "status": "SUCCESS",
    "message": ".اطلاعات حساب کاربری با موفقیت به روزرسانی شد"
  },
  "server_response_error": {
    "action": "UPDATE_PROFILE_RESPONSE",
    "status": "FAILED",
    "message": ".این نام کاربری یا ایمیل قبلاً توسط شخص دیگری انتخاب شده است"
  }
}
 ,
  "14_CHANGE_PASSWORD": {
    "client_request": {
      "action": "CHANGE_PASSWORD",
      "data": {
        "user_id": 42,
        "old_password": "OldPassword123",
        "new_password": "NewSecurePassword456"
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
        "user_id": 42
      }
    },
    "server_response_success": {
      "action": "GET_PURCHASE_HISTORY_RESPONSE",
      "status": "SUCCESS",
      "history": [
        {
          "book_id": 10,
          "purchase_date": "2026-03-15 14:30:00",
          "title": "شاهنامه",
          "author": "فردوسی",
          "price": 250000.0
        }
      ]
    }
  },
  "16_CHECK_BOOK_OWNERSHIP": {
    "client_request": {
      "action": "CHECK_BOOK_OWNERSHIP",
      "data": {
        "user_id": 42,
        "book_id": 10
      }
    },
    "server_response_success": {
      "action": "CHECK_BOOK_OWNERSHIP_RESPONSE",
      "book_id": 10,
      "status": "SUCCESS",
      "is_purchased": true,
      "is_in_cart" : false,
      "is_saved": true,
      "publisher_name": "انتشارات طوس",
      "rating": 4.8,
      "cover_image_path": "/home/username/BookClub_Storage/shahnameh.jpg"
    },
    "server_response_error_inactive": {
      "action": "CHECK_BOOK_OWNERSHIP_RESPONSE",
      "book_id": 10,
      "status": "FAILED",
      "message": ".این کتاب در حال حاضر غیرفعال یا ناموجود است"
    }
  },
  "17_GET_BOOK_PDF_PATH": {
    "client_request": {
      "action": "GET_BOOK_PDF_PATH",
      "data": {
        "user_id": 42,
        "book_id": 10
      }
    },
    "server_response_success": {
      "action": "GET_BOOK_PDF_PATH_RESPONSE",
      "book_id": 10,
      "status": "SUCCESS",
      "pdf_path": "/home/username/BookClub_Storage/shahnameh.pdf"
    },
    "server_response_error_access": {
      "action": "GET_BOOK_PDF_PATH_RESPONSE",
      "book_id": 10,
      "status": "FAILED",
      "message": ".شما دسترسی به این کتاب ندارید. ابتدا باید آن را خریداری کنید"
    },
    "server_response_error_missing": {
      "action": "GET_BOOK_PDF_PATH_RESPONSE",
      "book_id": 10,
      "status": "FAILED",
      "message": ".فایل پی‌دی‌اف این کتاب یافت نشد"
    }
  }
,
  "18_SEARCH_BOOKS": {
    "client_request": {
      "action": "SEARCH_BOOKS",
      "data": {
        "title": "شاهنامه",
        "author": "فردوسی",
        "publisher_name": "انتشارات"
      }
    },
    "server_response_success": {
      "action": "SEARCH_BOOKS_RESPONSE",
      "status": "SUCCESS",
      "books": [
        {
          "id": 10,
          "title": "شاهنامه",
          "author": "فردوسی",
          "genre": "Epic",
          "price": 250000.0,
          "discount_percentage": 10.0,
          "cover_image_path": "/home/username/BookClub_Storage/shahnameh.jpg",
          "publisher_name": "انتشارات طوس"
        }
      ]
    }
  }
,
  "19_ADD_COMMENT": {
    "client_request": {
      "action": "ADD_COMMENT",
      "book_id": 10,
      "user_id": 42,
      "text": "کتاب فوق‌العاده‌ای بود، خواندنش را توصیه می‌کنم.",
      "rating": 5
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
    "server_broadcast_on_success": {
      "action": "COMMENT_UPDATED",
      "book_id": 10,
      "type": "ADD"
    }
  },
  "20_EDIT_COMMENT": {
    "client_request": {
      "action": "EDIT_COMMENT",
      "comment_id": 257,
      "text": "کتاب بسیار عالی بود، متن آن روان و جذاب است.",
      "rating": 4
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
    "server_broadcast_on_success": {
      "action": "COMMENT_UPDATED",
      "comment_id": 257,
      "type": "EDIT"
    }
  },
  "21_DELETE_COMMENT": {
    "client_request": {
      "action": "DELETE_COMMENT",
      "comment_id": 257
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
    "server_broadcast_on_success": {
      "action": "COMMENT_UPDATED",
      "comment_id": 257,
      "type": "DELETE"
    }
  },
  "22_GET_COMMENTS": {
    "client_request": {
      "action": "GET_COMMENTS",
      "book_id": 10
    },
    "server_response_success": {
      "action": "GET_COMMENTS_RESPONSE",
      "status": "SUCCESS",
      "comments": [
        {
          "id": 257,
          "user_id": 4,
          "text": "کتاب فوق‌العاده‌ای بود، خواندنش را توصیه می‌کنم.",
          "rating": 5,
          "created_at": "2026-07-11T12:08:28",
          "updated_at": "2026-07-11T12:08:28",
          "username": "user42"
        }
      ]
    }
  }
,
  "23_ADD_TO_CART": {
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
  "24_REMOVE_FROM_CART": {
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
  "25_GET_CART": {
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
          "id": 10,
          "title": "شاهنامه",
          "author": "فردوسی",
          "price": 250000.0,
          "discount": 10.0,
          "coverImagePath": "/home/username/BookClub_Storage/shahnameh.jpg",
          "publisher_name": "Tech_Books",
          "isActive": true
        }
      ],
      "total_price": 250000.0,
      "discount": 25000.0,
      "final_price": 225000.0,
      "count": 1
    }
  },
  "26_FINALIZE_PURCHASE": {
  "client_request": {
    "action": "FINALIZE_PURCHASE",
    "data": {
      "user_id": 42,
      "client_final_price": 145000.0
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
    "message": ".قیمت یا موجودی کتاب‌ها تغییر یافته است. سبد خرید شما به‌روزرسانی می‌شود"
  }
}
  
,
  "27_GET_PURCHASED_BOOKS": {
    "client_request": {
      "action": "GET_PURCHASED_BOOKS",
      "data": {
        "user_id": 42
      }
    },
    "server_response_success": {
      "action": "GET_PURCHASED_BOOKS_RESPONSE",
      "status": "SUCCESS",
      "books": [
        {
          "id": 10,
          "title": "شاهنامه",
          "author": "فردوسی",
          "genre": "Epic",
          "price": 250000.0,
          "pdfPath": "/home/username/BookClub_Storage/shahnameh.pdf",
          "coverImagePath": "/home/username/BookClub_Storage/shahnameh.jpg"
        }
      ]
    },
    "server_response_failed": {
      "action": "GET_PURCHASED_BOOKS_RESPONSE",
      "status": "FAILED",
      "message": ".شناسه کاربر نامعتبر است"
    }
  },
  "28_SAVE_BOOK": {
    "client_request": {
      "action": "SAVE_BOOK",
      "user_id": 42,
      "book_id": 10
    },
    "server_response_success": {
      "action": "SAVE_BOOK_RESPONSE",
      "status": "SUCCESS",
      "message": ".کتاب ذخیره شد"
    },
    "server_response_error": {
      "action": "SAVE_BOOK_RESPONSE",
      "status": "ERROR",
      "message": ".خطا در ذخیره کتاب"
    }
  },
  "29_REMOVE_SAVED_BOOK": {
    "client_request": {
      "action": "REMOVE_SAVED_BOOK",
      "user_id": 42,
      "book_id": 10
    },
    "server_response_success": {
      "action": "REMOVE_SAVED_BOOK_RESPONSE",
      "status": "SUCCESS",
      "message": ".کتاب از لیست ذخیره شده حذف شد"
    },
    "server_response_error": {
      "action": "REMOVE_SAVED_BOOK_RESPONSE",
      "status": "ERROR",
      "message": ".خطا در حذف کتاب"
    }
  },
  "30_GET_SAVED_BOOKS": {
    "client_request": {
      "action": "GET_SAVED_BOOKS",
      "user_id": 42
    },
    "server_response_success": {
      "action": "GET_SAVED_BOOKS_RESPONSE",
      "status": "SUCCESS",
      "books": [
        {
          "id": 10,
          "title": "شاهنامه",
          "author": "فردوسی",
          "genre": "Epic",
          "coverImagePath": "/home/username/BookClub_Storage/shahnameh.jpg",
          "price": 250000.0,
          "discount": 10.0,
          "publisher_name": "نام ناشر (از فیلد name)"
        }
      ]
    }
  },
  "31_CREATE_SHELF": {
    "client_request": {
      "action": "CREATE_SHELF",
      "user_id": 42,
      "name": "کتاب‌های تاریخی"
    },
    "server_response_success": {
      "action": "CREATE_SHELF_RESPONSE",
      "status": "SUCCESS",
      "message": ".قفسه با موفقیت ایجاد شد"
    },
    "server_response_error": {
      "action": "CREATE_SHELF_RESPONSE",
      "status": "ERROR",
      "message": ".قفسه ای با این نام از قبل وجود دارد یا خطا رخ داده است"
    }
  },
  "32_RENAME_SHELF": {
    "client_request": {
      "action": "RENAME_SHELF",
      "shelf_id": 5,
      "new_name": "حماسی و تاریخی"
    },
    "server_response_success": {
      "action": "RENAME_SHELF_RESPONSE",
      "status": "SUCCESS",
      "message": ".نام قفسه تغییر کرد"
    },
    "server_response_error": {
      "action": "RENAME_SHELF_RESPONSE",
      "status": "ERROR",
      "message": ".این نام با یکی از قفسه های دیگر شما تداخل دارد یا قفسه یافت نشد"
    }
  },
  "33_DELETE_SHELF": {
    "client_request": {
      "action": "DELETE_SHELF",
      "shelf_id": 5
    },
    "server_response_success": {
      "action": "DELETE_SHELF_RESPONSE",
      "status": "SUCCESS",
      "message": ".قفسه حذف شد"
    },
    "server_response_error": {
      "action": "DELETE_SHELF_RESPONSE",
      "status": "ERROR",
      "message": ".خطا در حذف قفسه"
    }
  },
  "34_ADD_BOOK_TO_SHELF": {
    "client_request": {
      "action": "ADD_BOOK_TO_SHELF",
      "shelf_id": 5,
      "book_id": 10
    },
    "server_response_success": {
      "action": "ADD_BOOK_TO_SHELF_RESPONSE",
      "status": "SUCCESS",
      "message": ".کتاب به قفسه اضافه شد"
    },
    "server_response_error": {
      "action": "ADD_BOOK_TO_SHELF_RESPONSE",
      "status": "ERROR",
      "message": ".این کتاب از قبل در قفسه مورد نظر موجود است"
    }
  },
  "35_MOVE_BOOK_BETWEEN_SHELVES": {
    "client_request": {
      "action": "MOVE_BOOK_BETWEEN_SHELVES",
      "from_shelf": 5,
      "to_shelf": 6,
      "book_id": 10
    },
    "server_response_success": {
      "action": "MOVE_BOOK_BETWEEN_SHELVES_RESPONSE",
      "status": "SUCCESS",
      "message": ".کتاب با موفقیت منتقل شد"
    },
    "server_response_error": {
      "action": "MOVE_BOOK_BETWEEN_SHELVES_RESPONSE",
      "status": "ERROR",
      "message": ".کتاب از قبل در قفسه مقصد موجود بود و از قفسه فعلی حذف شد"
    }
  },
  "36_GET_SHELVES": {
    "client_request": {
      "action": "GET_SHELVES",
      "user_id": 42
    },
    "server_response_success": {
      "action": "GET_SHELVES_RESPONSE",
      "status": "SUCCESS",
      "shelves": [
        {
          "id": 5,
          "name": "حماسی و تاریخی"
        }
      ]
    }
  },
  "37_GET_SHELF_BOOKS": {
    "client_request": {
      "action": "GET_SHELF_BOOKS",
      "shelf_id": 5
    },
    "server_response_success": {
      "action": "GET_SHELF_BOOKS_RESPONSE",
      "status": "SUCCESS",
      "books": [
        {
          "id": 10,
          "title": "شاهنامه",
          "author": "فردوسی",
          "genre": "Epic",
          "coverImagePath": "/home/username/BookClub_Storage/shahnameh.jpg"
        }
      ]
    }
  }
,
  "38_GET_LAST_READ_PAGE": {
    "client_request": {
      "action": "GET_LAST_READ_PAGE",
      "user_id": 42,
      "book_id": 10
    },
    "server_response_success": {
      "action": "GET_LAST_READ_PAGE_RESPONSE",
      "status": "SUCCESS",
      "page": 12
    }
  },
  "39_UPDATE_LAST_READ_PAGE": {
    "client_request": {
      "action": "UPDATE_LAST_READ_PAGE",
      "user_id": 42,
      "book_id": 10,
      "page": 13
    },
    "server_response_success": {
      "action": "UPDATE_LAST_READ_PAGE_RESPONSE",
      "status": "SUCCESS",
      "message": ".آخرین صفحه ذخیره شد"
    },
    "server_response_error": {
      "action": "UPDATE_LAST_READ_PAGE_RESPONSE",
      "status": "ERROR",
      "message": ".خطا در ذخیره صفحه"
    }
  }
,
  "40_GET_PUBLISHER_PROFILE": {
    "client_request": {
      "action": "GET_PUBLISHER_PROFILE",
      "publisher_id": 5
    },
    "server_response_success": {
      "action": "GET_PUBLISHER_PROFILE_RESPONSE",
      "status": "SUCCESS",
      "profile": {
        "id": 5,
        "username": "ghoqnoos_pub",
        "name": "انتشارات ققنوس",
        "email": "info@ghoqnoos.com"
      }
    },
    "server_response_failed": {
      "action": "GET_PUBLISHER_PROFILE_RESPONSE",
      "status": "FAILED",
      "message": ".ناشر یافت نشد یا نقش کاربر ناشر نیست"
    }
  },
  "41_UPDATE_PUBLISHER_PROFILE": {
    "client_request": {
      "action": "UPDATE_PUBLISHER_PROFILE",
      "publisher_id": 5,
      "info": {
        "username": "ghoqnoos_press",
        "name": "نشر ققنوس جدید",
        "email": "new_info@ghoqnoos.com"
      }
    },
    "server_response_success": {
      "action": "UPDATE_PUBLISHER_PROFILE_RESPONSE",
      "status": "SUCCESS",
      "message": ".اطلاعات پروفایل ناشر با موفقیت به روزرسانی شد"
    },
    "server_response_failed": {
      "action": "UPDATE_PUBLISHER_PROFILE_RESPONSE",
      "status": "FAILED",
      "message": ".خطا در ویرایش اطلاعات. این نام کاربری، نام یا ایمیل قبلاً توسط شخص دیگری انتخاب شده یا ناشر یافت نشد"
    }
  }
,
  "42_ADD_BOOK": {
    "client_request": {
      "action": "ADD_BOOK",
      "title": "شاهنامه فردوسی",
      "author": "فردوسی",
      "genre": "حماسی",
      "description": "نسخه کامل شاهنامه به تصحیح مسکو",
      "price": 250000.0,
      "discountPercent": 10.0,
      "publisher_id": 5,
      "publisher_pdf_path": "C:/Users/Publisher/Documents/shahnameh.pdf",
      "publisher_cover_path": "C:/Users/Publisher/Documents/shahnameh_cover.jpg"
    },
    "server_response_success": {
      "action": "ADD_BOOK_RESPONSE",
      "status": "SUCCESS",
      "message": ".کتاب با موفقیت کپی و در پایگاه داده ثبت شد"
    },
    "server_response_failed_validation": {
      "action": "ADD_BOOK_RESPONSE",
      "status": "FAILED",
      "message": ".اطلاعات اجباری کتاب یا مسیر فایل پی دی اف ارسال نشده است"
    },
    "server_response_failed_pdf_copy": {
      "action": "ADD_BOOK_RESPONSE",
      "status": "FAILED",
      "message": ".امکان کپی و ثبت فایل پی دی اف روی هارد دیسک سرور وجود ندارد"
    },
    "server_response_failed_cover_copy": {
      "action": "ADD_BOOK_RESPONSE",
      "status": "FAILED",
      "message": ".امکان کپی و ثبت عکس کاور روی هارد دیسک سرور وجود ندارد"
    },
    "server_response_failed_db": {
      "action": "ADD_BOOK_RESPONSE",
      "status": "FAILED",
      "message": ".خطا در ثبت اطلاعات کتاب در پایگاه داده"
    }
  },
  "43_UPDATE_BOOK": {
    "client_request": {
      "action": "UPDATE_BOOK",
      "book_id": 10,
      "title": "شاهنامه فردوسی (ویرایش جدید)",
      "author": "فردوسی",
      "genre": "حماسی",
      "description": "نسخه ویراسته جدید همراه با مقدمه",
      "price": 300000.0,
      "discountPercent": 15.0,
      "pdfPath": "1700000000000_shahnameh.pdf",
      "coverImagePath": "1700000000000_shahnameh_cover.jpg",
      "publisher_pdf_path": "C:/Users/Publisher/Downloads/shahnameh_v2.pdf",
      "publisher_cover_path": "C:/Users/Publisher/Downloads/shahnameh_v2_cover.jpg"
    },
    "server_response_success": {
      "action": "UPDATE_BOOK_RESPONSE",
      "status": "SUCCESS",
      "message": ".کتاب با موفقیت ویرایش و فایل های جدید جایگزین شدند"
    },
    "server_response_failed_invalid_id": {
      "action": "UPDATE_BOOK_RESPONSE",
      "status": "FAILED",
      "message": ".شناسه کتاب نامعتبر است"
    },
    "server_response_failed_pdf_copy": {
      "action": "UPDATE_BOOK_RESPONSE",
      "status": "FAILED",
      "message": ".امکان کپی و جایگزینی فایل پی دی اف جدید روی سرور وجود ندارد"
    },
    "server_response_failed_cover_copy": {
      "action": "UPDATE_BOOK_RESPONSE",
      "status": "FAILED",
      "message": ".امکان کپی و جایگزینی عکس کاور جدید روی سرور وجود ندارد"
    },
    "server_response_failed_db": {
      "action": "UPDATE_BOOK_RESPONSE",
      "status": "FAILED",
      "message": ".خطا در ثبت تغییرات کتاب در پایگاه داده سرور"
    }
  },
  "44_SET_BOOK_DISCOUNT": {
    "client_request": {
      "action": "SET_BOOK_DISCOUNT",
      "book_id": 10,
      "publisher_id": 5,
      "discountPercent": 20.0
    },
    "server_response_success": {
      "action": "SET_BOOK_DISCOUNT_RESPONSE",
      "status": "SUCCESS",
      "message": ".تخفیف با موفقیت اعمال و محاسبه شد"
    },
    "server_response_error": {
      "action": "SET_BOOK_DISCOUNT_RESPONSE",
      "status": "ERROR",
      "message": ".خطا در اعمال تخفیف"
    }
  },
  "45_SET_BOOK_ACTIVE_STATE": {
    "client_request": {
      "action": "SET_BOOK_ACTIVE_STATE",
      "book_id": 10,
      "publisher_id": 5,
      "active": true
    },
    "server_response_success": {
      "action": "SET_BOOK_ACTIVE_STATE_RESPONSE",
      "status": "SUCCESS",
      "message": ".کتاب فعال شد"  "OR"  ".کتاب غیرفعال شد"
    },
    "server_response_error": {
      "action": "SET_BOOK_ACTIVE_STATE_RESPONSE",
      "status": "ERROR",
      "message": ".خطا در تغییر وضعیت کتاب"
    }
  },
  "46_GET_PUBLISHER_BOOKS": {
    "client_request": {
      "action": "GET_PUBLISHER_BOOKS",
      "publisher_id": 5
    },
    "server_response_success": {
      "action": "GET_PUBLISHER_BOOKS_RESPONSE",
      "status": "SUCCESS",
      "books": [
        {
          "id": 10,
          "title": "شاهنامه فردوسی",
          "author": "فردوسی",
          "genre": "حماسی",
          "description": "نسخه کامل شاهنامه به تصحیح مسکو",
          "price": 250000.0,
          "discountPercent": 10.0,
          "coverImagePath": "/home/user/BookClub_Storage/1700000000000_shahnameh_cover.jpg",
          "pdfPath": "/home/user/BookClub_Storage/1700000000000_shahnameh.pdf",
          "isActive": 1
        }
      ]
    }
  }
,
  "47_GET_PUBLISHER_STATS": {
    "client_request": {
      "action": "GET_PUBLISHER_STATS",
      "publisher_id": 5
    },
    "server_response_success": {
      "action": "GET_PUBLISHER_STATS_RESPONSE",
      "status": "SUCCESS",
      "stats": {
        "totalBooks": 12,
        "totalRevenue": 2450000.0,
        "booksRatings": [
          {
            "book_id": 10,
            "title": "شاهنامه فردوسی",
            "avgRating": 4.8
          },
          {
            "book_id": 11,
            "title": "دیوان حافظ",
            "avgRating": 4.9
          }
        ],
        "bestSellers": [
          {
            "book_id": 11,
            "title": "دیوان حافظ",
            "salesCount": 150
          },
          {
            "book_id": 10,
            "title": "شاهنامه فردوسی",
            "salesCount": 120
          }
        ],
        "worstSellers": [
          {
            "book_id": 15,
            "title": "کتاب نمونه آزمایشی",
            "salesCount": 0
          },
          {
            "book_id": 12,
            "title": "گلستان سعدی",
            "salesCount": 5
          }
        ]
      }
    }
  }
,
  "48_GET_ALL_USERS": {
    "client_request": {
      "action": "GET_ALL_USERS"
    },
    "server_response_success": {
      "action": "GET_ALL_USERS_RESPONSE",
      "status": "SUCCESS",
      "users": [
        {
          "id": 1,
          "username": "admin_user",
          "role": "admin",
          "is_blocked": 0,
          "registration_date": "2026-01-15 10:30:00"
        },
        {
          "id": 2,
          "username": "publisher_test",
          "role": "publisher",
          "is_blocked": 1,
          "registration_date": "2026-02-20 14:45:12"
        }
      ]
    }
  },
  "49_GET_USER_DETAILS": {
    "client_request": {
      "action": "GET_USER_DETAILS",
      "user_id": 2
    },
    "server_response_success": {
      "action": "GET_USER_DETAILS_RESPONSE",
      "status": "SUCCESS",
      "user": {
        "id": 2,
        "username": "publisher_test",
        "role": "publisher",
        "is_blocked": 1,
        "security_question": "نام اولین مدرسه شما چیست؟",
        "registration_date": "2026-02-20 14:45:12"
      }
    },
    "server_response_error": {
      "action": "GET_USER_DETAILS_RESPONSE",
      "status": "ERROR",
      "message": ".کاربر یافت نشد"
    }
  },
  "50_SEARCH_USERS": {
    "client_request": {
      "action": "SEARCH_USERS",
      "keyword": "test",
      "role": "publisher",
      "blocked": -1,
      "register_date": "2026-02"
    },
    "server_response_success": {
      "action": "SEARCH_USERS_RESPONSE",
      "status": "SUCCESS",
      "users": [
        {
          "id": 2,
          "username": "publisher_test",
          "role": "publisher",
          "is_blocked": 1,
          "registration_date": "2026-02-20 14:45:12"
        }
      ]
    }
  }
}







    
      
