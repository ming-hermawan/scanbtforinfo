# Scan Bluetooth's Info Around You
Copyright (C) 2025 Hermawan <minghermawan@yahoo.com>

LICENSE: GNU General Public License (GPL) v3.0

I'd love to hear your thoughts; whether it's feedback, suggestions, or ideas for improvement. 

If you enjoy the project, please don't forget to give it a ⭐ 

Your support mean a lot to me and boost me to keep improving it.

[Linkedin](https://www.linkedin.com/in/hermawan-ho-a3801194/)

[Facebook](https://web.facebook.com/minghermawan)

## 1. Introduction
Get info of Bluetooth devices around you and save it to SQLite database.ase.

After data is gathered and stored in the SQlite database, use it for anything like AI analysis or more.

Why SQLite database? It's lightweight and easier to transform data into another database with ETL tools.

## 2. Note:
When the application starts running, it will run and gather data forever until you break it.

When running, this application will gather info and saved in SQLite database name is bt.db.

This application still in alpha phase, so maybe still has a lot of bugs, so please inform me at minghermawan@yahoo.com for bugs and I really appreciate if you give me your review & suggestion about this application, either good or bad, so I can improve this application more.

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License v3.0.

For everyone who checks my codes, I'm not following C standard codes intentionally, with hope everyone can recognize my code style if get the codes from somewhere else.

## 3. How to Run:
### Linux
1). Please make sure Bluetooth library (libbluetooth-dev for Debian family, bluez-libs-devel for RHEL family) installed in your Linux environment.

2). Compile with C compiler.

Example With GCC compiler:

`gcc btinfo.c dbsqlite.c scanbtforinfo.c -lsqlite3 -lbluetooth -o scanbtforinfo;`

3). Run with super user:

Example:

`sudo ./scanbtinfo;`
