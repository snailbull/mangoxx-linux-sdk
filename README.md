# 固件开发仿真

[![standard-readme compliant](https://img.shields.io/badge/readme%20style-standard-brightgreen.svg?style=flat-square)](https://github.com/RichardLitt/standard-readme)

标准 Readme 样式

README 文件是人们通常最先看到的第一个东西。它应该告诉人们为什么要使用、如何安装、以及如何使用你的代码。README 文件标准化能够使得创建和维护 README 文件更加简单。 毕竟，要写好一个文档不是那么容易的。

本仓库包含以下内容：

1. 一个标准的 README 文件应该是什么样子的[规范](spec.md)。
2. 一个用于维护 REAME 文件的语法提示工具的链接 ([正在进行中](https://github.com/RichardLitt/standard-readme/issues/5)).
3. 一个创建标准 README 的[生成器](https://github.com/RichardLitt/generator-standard-readme)。
4. 一个指向该规范的[徽章](#徽章)。
5. [标准 README 的实例](example-readmes/) - 比如你正在读的这个文件。
标准 Readme 是为了开源组件 设计的。尽管它的[背景](#背景)是为了服务于 Node 和 npm 项目, 它同时也可以应用到其他编程语言和包管理器中去。

## 内容列表

- [背景](#背景)
- [安装](#安装)
- [使用](#使用)
	- [生成器](#生成器)
- [徽章](#徽章)
- [Readme 示例](#Readme-示例)
- [相关仓库](#相关仓库)
- [维护者](#维护者)
- [如何贡献](#如何贡献)
- [使用许可](#使用许可)

## 背景

`标准化Readme` 最开始因为 [@maxogden](https://github.com/maxogden) 在项目[feross/standard](https://github.com/feross/standard)的[这个Issue](https://github.com/feross/standard/issues/141)中提出, 是否标准化 README 会有助于帮助大家. 很多人在仓库[zcei's standard-readme](https://github.com/zcei/standard-readme/issues/1) 就这个想法进行了讨论. 在我维护仓库 [IPFS](https://github.com/ipfs)的时候, 我需要在这个组织中推广标准化的 Readme，因此这个项目也就从这开始了.

> 如果你的文档是完整的， 那么使用你代码的人就不用再去看代码了。这非常的重要。它使得你可以分离接口文档与具体实现。它意味着你可修改实现的代码而保持接口与文档不变。

> 请记住： 是文档而非代码，定义了一个模块的功能。

~ [Ken Williams, Perl Hackers](http://mathforum.org/ken/perl_modules.html#document)

写 README 从某种程度上来说相当不易，一直维护下去更是难能可贵。如果可以减少这个过程，则可以让写代码与修改代码更容易，使得是否在说明中指明一处需改有无必要更加清楚， 你可以花费更少的时间来考虑是否你最初的文档是否需要更新， 你可以分配更多的时间来写代码而非维护文档。

同时， 标准化在某些别的地方也有好处。有了标准化，用户就可以花费更少的时间来搜索他们需要的信息， 他们同时可以做一个工具来从描述中搜集信息，自动跑示例代码，检查授权协议等等。

这个仓库的目标是：


1. 一个定义良好的 **规范**。在仓库中的位置是[spec.md](spec.md)。它是一个一直在持续优化的文档，欢迎您提 Issue 讨论其中的变化。
2. 一个**示例 README**。 这个Readme 完全遵从 Standard-readme， 而且在 `example-readmes` 文件夹里有更多的示例.
3. 一个 **语法提示器** 用来提示在Readme 中的语法错误. 请参考 [tracking issue](https://github.com/RichardLitt/standard-readme/issues/5).
4. 一个 **生成器** 用来快速搭建新的 README 的框架。 请参考 [generator-standard-readme](https://github.com/RichardLitt/generator-standard-readme).
5. 一个 **标识准守规范的徽章**。 请参考 [徽章](#徽章).

## 安装

这个项目使用 [node](http://nodejs.org) 和 [npm](https://npmjs.com). 请确保你本地安装了它们。
```sh
$ npm install --global standard-readme-spec
```

## 使用说明

这只是一个文档包， 你可以打印出[spec.md](spec.md)到输出窗口。

```sh
$ standard-readme-spec
# Prints out the standard-readme spec
```

### 生成器


想要使用生成器的话, 请看[generator-standard-readme](https://github.com/RichardLitt/generator-standard-readme). 
有一个全局的可执行文件来运行包里的生成器， 生成器的别名叫 `standard-readme` 。

## 徽章
如果你的项目遵循 Standard-Readme 而且项目位于 Github上，非常希望你能把这个徽章加入你的项目。 它可以更多的人访问到这个项目，而且采纳 Stand-README。 加入徽章**并非强制的**。 

[![standard-readme compliant](https://img.shields.io/badge/readme%20style-standard-brightgreen.svg?style=flat-square)](https://github.com/RichardLitt/standard-readme)

为了加入徽章到 Markdown 文本里面，可以使用以下代码：

```
[![standard-readme compliant](https://img.shields.io/badge/readme%20style-standard-brightgreen.svg?style=flat-square)](https://github.com/RichardLitt/standard-readme)
```

## Readme 示例

想了解我们建议的规范是如何被应用的， 请参考[example-readmes](example-readmes/)。

## 相关仓库

- [Art of Readme](https://github.com/noffle/art-of-readme) - 写高质量 README 的艺术。
- [open-source-template](https://github.com/davidbgk/open-source-template/) - 一个鼓励参与开源的 README 模板。

## 维护者

[@snailbull](https://github.com/snailbull).

## 如何贡献

非常欢迎你的加入! [提一个Issue](https://github.com/RichardLitt/standard-readme/issues/new) 或者提交一个 Pull R equest.


标准 Readme 遵循 [Contributor Covenant](http://contributor-covenant.org/version/1/3/0/) 行为规范.

### 贡献者

感谢以下参与项目的人：
<a href="graphs/contributors"><img src="https://opencollective.com/standard-readme/contributors.svg?width=890&button=false" /></a>


## 使用许可

[MIT](LICENSE) © Richard Littauer
