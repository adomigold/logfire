Perfect — since your tool is now able to parse Apache and Nginx logs, search, and output in multiple formats including JSON, here's a `README.md` for your project **Logfire**:

---

````markdown
# 🔥 Logfire

**Logfire** is a lightweight, blazing-fast command-line log processor built in C. It allows you to parse, search, and format logs (such as Apache and Nginx access logs) into readable or structured formats like JSON and CSV — all without the bloat of heavy tools like ELK.

---

## 🚀 Features

- 📄 Parse standard Apache/Nginx access logs
- 🔍 Search logs by keyword (method, IP, URL, timestamp, user agent)
- 📤 Output results in:
  - Plain text
  - CSV
  - JSON (with proper escaping)
- 📁 Optional output to file
- 🧩 Easy to embed into other C tools or scripts
- ⚡️ Extremely fast and portable

---

## 🛠️ Build

You need a C compiler like `gcc`.

```bash
gcc -o logfire src/logfire.c
````

If using multiple source files:

```bash
gcc -o logfire src/logfire.c src/parser.c src/cli.c -Wall
```

---

## 📦 Usage

```bash
./logfire --log access.log --search "OPTIONS" --format json --output result.json
```

### ✅ Options

| Flag       | Description                                      |
| ---------- | ------------------------------------------------ |
| `--log`    | Path to the log file                             |
| `--search` | Keyword to search (method, URL, IP, etc.)        |
| `--format` | Output format: `text`, `json`, or `csv`          |
| `--output` | (Optional) Path to output file instead of stdout |

---

## 📚 Example

### Input (Apache access log):

```
198.46.149.143 - - [20/May/2015:12:05:13 +0000] "GET /blog/page.html HTTP/1.1" 200 9316 "Tiny Tiny RSS"
```

### Output (JSON):

```json
[
  {
    "timestamp": "20/May/2015:12:05:13 +0000",
    "ip": "198.46.149.143",
    "method": "GET",
    "url": "/blog/page.html",
    "status": 200,
    "userAgent": "Tiny Tiny RSS"
  }
]
```

---

## 🧪 Test

```bash
./logfire --log sample.log --format json
./logfire --log sample.log --search "POST" --format csv
```

---

## 🧰 Roadmap

* [ ] Regex-based advanced filtering
* [ ] Support log rotation
* [ ] Log summarization / stats
* [ ] Add support for custom log formats
* [ ] Support other systems logs (eg. MYSQL, POSTGRESQL logs)

---

## 👨‍💻 Author

Made with ❤️ by Adolph Mapunda
[GitHub](https://github.com/adomigold) • [LinkedIn](https://www.linkedin.com/in/adolph-Mapunda-106474178/)

---

## ⚖️ License

 AGPL-3.0 License

```
Logfire is licensed under the [GNU AGPL v3.0](./LICENSE).
```
