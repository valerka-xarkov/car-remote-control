const path = require('path');
const htmlWebpackPlugin = require('html-webpack-plugin');

const dest = './dist';


module.exports = {
  entry: './src/ts/main.ts',
  module: {
    rules: [
      {
        test: /\.ts$/,
        use: 'ts-loader',
        exclude: /node_modules/,
      },
      {
        test: /\.css$/i,
        use: ['style-loader', 'css-loader'],
      },
    ],
  },
  resolve: {
    extensions: ['.ts'],
  },

  optimization: {
    minimize: false,
  },
  output: {
    filename: 'index.js',
    path: path.resolve(__dirname, dest),
  },
  plugins: [
    new htmlWebpackPlugin({
      inject: false,
      template: 'src/index.html',
    })
  ],
  watch: true
};
