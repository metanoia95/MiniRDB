# MiniRDB — 미니 관계형 데이터베이스 엔진 구현

> C++17로 직접 설계·구현한 소형 RDBMS 서버

---

## 프로젝트 개요

| 항목 | 내용 |
|------|------|
| **저장소** | [github.com/metanoia95/MiniRDB](https://github.com/metanoia95/MiniRDB) |
| **언어** | C++ 17 (98.5%) |
| **빌드 환경** | CMake + Visual Studio (`.slnx`, `.vcxproj`) |
| **네트워크** | Winsock2 기반 TCP 소켓 서버 (포트 27015) |
| **커밋 수** | 15 Commits |

SQL 파싱부터 스토리지 관리, TCP 서버까지 관계형 데이터베이스의 핵심 레이어를 처음부터 직접 설계하고 구현한 프로젝트입니다. 단순 자료구조 구현을 넘어, 실제 RDBMS가 쿼리를 처리하는 전체 파이프라인을 직접 경험하는 것을 목표로 했습니다.

---

## 기술 스택

- **언어**: C++17
- **빌드**: CMake, Visual Studio MSBuild
- **네트워크**: Winsock2 (Windows 소켓 API)
- **자료구조**: B-Tree 인덱스 (자체 구현)
- **디자인 패턴**: Visitor 패턴, Strategy 패턴

---

## 시스템 아키텍처

```
클라이언트 요청 (TCP)
        │
        ▼
  ServerSocket          ← Winsock2 TCP 서버 (포트 27015)
        │
        ▼
      App               ← DB 엔진 진입점 (CLI 테스트 / 자동 쿼리 테스트)
        │
        ▼
  [SQL 파싱 파이프라인]
  Tokenizer → Parser → AST
                         │
                         ▼
                    AstVisitor         ← Visitor 패턴으로 쿼리 실행
                         │
              ┌──────────┴──────────┐
              ▼                     ▼
         Schema / Table       StorageManager
         (관계형 모델)          (파일 I/O 관리)
              │                     │
              ▼                     ▼
           BTree              FileIoStrategy   ← Strategy 패턴
         (인덱스)              (I/O 전략)
              │
              ▼
          ResultSet            ← 쿼리 결과 집합
```

---

## 구성 모듈 상세

### SQL 파싱 레이어

SQL 쿼리 문자열을 실행 가능한 형태로 변환하는 3단계 파이프라인을 직접 구현했습니다.

**Tokenizer** (`Tokenizer.h` / `Tokenizer.cpp`)는 입력 문자열을 토큰 단위로 분리합니다. **Parser** (`parser.h` / `parser.cpp`)는 토큰 스트림을 읽어 문법에 맞게 **AST(Abstract Syntax Tree)** 를 생성합니다. AST 노드 구조는 `Ast.h` / `Ast.cpp`에 정의되어 있으며, 이를 **Visitor 패턴**(`AstVisitor.h`)으로 순회하여 실제 쿼리를 실행합니다.

### 데이터 모델 레이어

`Schema.h` / `Schema.cpp`는 테이블 스키마(컬럼 이름, 타입 등)를 정의하고, `Table.h` / `Table.cpp`는 실제 데이터 행(row)을 관리합니다. `types.h`에는 프로젝트 전반에서 사용되는 공통 타입이 선언되어 있습니다.

### 스토리지 레이어

`StorageManager.h` / `StorageManager.cpp`가 디스크 I/O 전반을 관리합니다. 파일 입출력 방식은 `FileIoStrategy.h`의 **Strategy 패턴**으로 추상화하여 I/O 전략을 유연하게 교체할 수 있도록 설계했습니다.

### 인덱스

`BTree.h`에 B-Tree 기반 인덱스를 구현하여 빠른 키 탐색을 지원합니다. (이전 B+ Tree 프로젝트에서 쌓은 경험을 실제 DB 엔진에 적용)

### 결과 집합 & 비교 전략

`ResultSet.h`는 쿼리 실행 결과를 담는 구조체이고, `ComparisonStrategy.h`는 WHERE 절 등 조건 비교 로직을 Strategy 패턴으로 분리하여 다양한 비교 연산을 확장 가능하게 했습니다.

### 네트워크 레이어

`ServerSocket.h` / `ServerSocket.cpp`는 Winsock2를 이용해 TCP 서버를 구현합니다. 포트 27015에서 Listen 상태로 대기하며 클라이언트 요청을 수신합니다. `StatusCode.h`는 서버 응답 코드를 정의합니다.

---

## 진입점 및 테스트 구조

```cpp
App app;
app.runQueryListTest(); // 미리 정의된 쿼리 목록 자동 실행
app.runCliTest();       // CLI 환경에서 직접 쿼리 입력 테스트

ServerSocket socket = ServerSocket("27015");
if (socket.init()) {
    socket.run(); // TCP 서버 시작
}
```

두 가지 테스트 모드를 지원합니다. `runQueryListTest()`는 사전 정의된 쿼리 시나리오를 자동 실행해 회귀 테스트처럼 동작하고, `runCliTest()`는 직접 쿼리를 타이핑하며 인터랙티브하게 확인할 수 있습니다.

---

## 적용한 디자인 패턴

| 패턴 | 적용 위치 | 목적 |
|------|-----------|------|
| **Visitor** | `AstVisitor.h` | AST 순회와 실행 로직을 AST 구조로부터 분리 |
| **Strategy** | `FileIoStrategy.h` | 파일 I/O 방식을 런타임에 교체 가능하도록 추상화 |
| **Strategy** | `ComparisonStrategy.h` | WHERE 절 비교 연산을 확장 가능한 형태로 분리 |

---

## 기술적 도전과 해결

**1. SQL 파서를 직접 구현**

라이브러리 없이 Tokenizer → Parser → AST의 3단계 파싱 파이프라인을 직접 설계했습니다. 문법 오류 처리, 연산자 우선순위, 토큰 타입 분류 등 파서 이론을 실제 코드로 구현하며 컴파일러 설계 원리를 체득했습니다.

**2. 인터페이스 분리를 통한 확장성 확보**

I/O 전략과 비교 연산을 Strategy 패턴으로 추상화하여, 새로운 저장 방식이나 조건 연산자를 추가할 때 기존 코드를 수정하지 않고 확장할 수 있는 구조를 만들었습니다.

**3. B-Tree 인덱스 통합**

이전 B+ Tree 프로젝트에서 직접 구현한 트리 자료구조를 DB 엔진의 인덱스 레이어로 통합하여, 자료구조 지식이 실제 시스템에 어떻게 적용되는지 연결했습니다.

**4. Winsock 기반 TCP 서버 구현**

DB 엔진을 독립된 서버로 분리하고 TCP 소켓으로 쿼리를 수신하는 구조를 구현하여, 단순 라이브러리가 아닌 실제 서버 프로세스 형태의 RDBMS를 완성했습니다.

---

## 배운 점

- SQL 파싱 파이프라인(Tokenizer → Parser → AST → Visitor)을 처음부터 구현하며 컴파일러 프론트엔드 원리를 직접 경험
- Visitor / Strategy 등 GoF 디자인 패턴을 실제 시스템 설계에 적용하고 그 필요성을 몸으로 이해
- 자체 구현한 B-Tree를 DB 인덱스로 통합하며 자료구조가 실제 시스템에서 어떻게 쓰이는지 연결
- Winsock2 소켓 프로그래밍으로 네트워크 레이어까지 직접 구현한 end-to-end 서버 개발 경험

---

*GitHub: [https://github.com/metanoia95/MiniRDB](https://github.com/metanoia95/MiniRDB)*
